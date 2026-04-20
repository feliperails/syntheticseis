#include "RegularGridWorker.h"

#include <QTime>
#include <QElapsedTimer>
#include <QDebug>
#include <QVTKOpenGLNativeWidget.h>

#include <array>
#include <future>
#include <cstdint>
#include <cstring>
#include <cmath>

#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkScalarBarActor.h>
#include <vtkImageData.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageProperty.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPlane.h>


namespace syntheticSeismic {
namespace widgets {

RegularGridWorker::RegularGridWorker(const std::shared_ptr<domain::RegularGrid<double>>& regularGrid,
                                     const QString& scalarBarTitle,
                                     bool useNearestInterpolation) :
    m_regularGrid(regularGrid),
    m_scalarBarTitle(scalarBarTitle),
    m_useNearestInterpolation(useNearestInterpolation)
{
}

RegularGridWorker::~RegularGridWorker()
{
}

void RegularGridWorker::run()
{
    QElapsedTimer timer;
    timer.start();

    buildGrid();

    const qint64& elapsedMs = timer.elapsed();
    QTime time = QTime(0, 0).addMSecs(static_cast<int>(elapsedMs));
    qInfo().noquote() << "RegularGridWorker::buildGrid finished in" << time.toString("hh:mm:ss");

    m_currentSteps = 100;
    emit stepProgress(m_currentSteps);

    emit finished();
}

void RegularGridWorker::buildGrid()
{
    m_currentSteps = 0;
    emit stepProgress(m_currentSteps);

    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

    // Seismic viewer pattern borrowed from CimaView3D (cs2i-senai-cimatec):
    // the data is rendered as a set of axis-aligned slice planes textured
    // with the amplitudes on that plane, not as a 3D volume — see
    // vtkImageResliceMapper + vtkImageSlice below. The dialog's Z-zoom
    // spinbox scales the actors for vertical exaggeration; the "trim empty
    // slices" checkbox swaps the mappers' input between the full view and a
    // trimmed domain-level copy (see m_imageDataFull vs imageDataTrimmed
    // below).
    //
    // The grid is anchored at world origin (0, 0, 0) by construction:
    // RotateVolumeCoordinate translates + rotates the input volumes so
    // their reference corner sits at origin and the min-rectangle is
    // axis-aligned; VolumeToRegularGrid then indexes cells by
    // point.x / cellSize (cell 0,0,0 → world 0,0,0). rectanglePoints
    // describes the ORIGINAL pre-translation footprint and is only relevant
    // for SEG-Y georeferencing, not for rendering.
    //
    // Cell sizes are applied as VTK spacing so world extents reflect the
    // real anisotropy (e.g. 25m × 25m × 2m). Z spacing is negated so depth
    // (increasing k) renders downward, matching the EclipseGridWorker
    // convention used by VtkViewerDialog slicing.

    const size_t dimZ = m_regularGrid->getNumberOfCellsInZ();
    const size_t effectiveDimZ = computeEffectiveDimZ(*m_regularGrid);
    if (effectiveDimZ != dimZ)
    {
        qInfo().noquote() << "RegularGridWorker: trimmed" << (dimZ - effectiveDimZ)
                          << "trailing empty Z slices (kept" << effectiveDimZ << "of" << dimZ << ").";
    }

    const bool didTrim = (effectiveDimZ != dimZ);
    auto trimmedGrid = didTrim
        ? copyTrimmedZ(*m_regularGrid, effectiveDimZ)
        : m_regularGrid;

    // Helper: allocate a vtkImageData whose geometry matches the given grid.
    auto makeImageData = [](const domain::RegularGrid<double>& grid) {
        auto img = vtkSmartPointer<vtkImageData>::New();
        img->SetDimensions(
            static_cast<int>(grid.getNumberOfCellsInX()),
            static_cast<int>(grid.getNumberOfCellsInY()),
            static_cast<int>(grid.getNumberOfCellsInZ()));
        img->SetOrigin(0.0, 0.0, 0.0);
        img->SetSpacing(
            grid.getCellSizeInX(),
            grid.getCellSizeInY(),
            -grid.getCellSizeInZ());
        img->AllocateScalars(VTK_FLOAT, 1);
        return img;
    };

    // Build the full ImageData always (exposed via getFullImageData() for
    // the dialog's trim-off toggle). When no trim happened, the trimmed
    // view reuses it verbatim — no second fill.
    m_imageDataFull = makeImageData(*m_regularGrid);
    vtkSmartPointer<vtkImageData> imageDataTrimmed;
    FillResult colorFill;
    if (didTrim)
    {
        (void)fillImageDataFromGrid(*m_regularGrid, m_imageDataFull, 0, 48);
        imageDataTrimmed = makeImageData(*trimmedGrid);
        colorFill = fillImageDataFromGrid(*trimmedGrid, imageDataTrimmed, 50, 98);
    }
    else
    {
        colorFill = fillImageDataFromGrid(*m_regularGrid, m_imageDataFull, 0, 98);
        imageDataTrimmed = m_imageDataFull;
    }

    // Mappers bind to the trimmed (or full, if no trim) view by default.
    auto imageData = imageDataTrimmed;
    double minValue = colorFill.minValue;
    double maxValue = colorFill.maxValue;

    // Seismic "seismic" colormap: symmetric blue → white → red around zero
    // (CimaView3D's default for amplitude). For amplitude we clamp the range
    // to ±max(|min|, |max|) so the white sits exactly at zero; for lithology
    // or depth where the range isn't zero-centred we just span [min, max].
    const bool symmetricAroundZero = (minValue < 0.0) && (maxValue > 0.0);
    double cmin = minValue;
    double cmax = maxValue;
    if (symmetricAroundZero)
    {
        const double clim = std::max(std::abs(minValue), std::abs(maxValue));
        cmin = -clim;
        cmax = clim;
    }
    const double cmid = (cmin + cmax) / 2.0;
    const int numIntermediateSteps = 8;

    vtkNew<vtkColorTransferFunction> colorTransfer;
    colorTransfer->SetColorSpaceToRGB();
    for (int i = 0; i <= numIntermediateSteps; ++i) {
        const double t = static_cast<double>(i) / numIntermediateSteps;
        const double val = cmin + t * (cmid - cmin);
        colorTransfer->AddRGBPoint(val, t, t, 1.0);
    }
    for (int i = 1; i <= numIntermediateSteps; ++i) {
        const double t = static_cast<double>(i) / numIntermediateSteps;
        const double val = cmid + t * (cmax - cmid);
        colorTransfer->AddRGBPoint(val, 1.0, 1.0 - t, 1.0 - t);
    }

    // Opaque slices: the frontmost plane shows solid amplitudes instead of
    // blending with whatever plane is behind it. Useful when three slices
    // cross and the user wants to read the exact colour of the frontmost one
    // rather than a mix.
    vtkNew<vtkImageProperty> imageProperty;
    imageProperty->SetLookupTable(colorTransfer);
    imageProperty->UseLookupTableScalarRangeOn();
    // Lithology is a categorical field — each integer code maps to a distinct
    // rock type, so linear interpolation between adjacent cells would invent
    // nonexistent intermediate codes (and colours). Use nearest-neighbour so
    // cell boundaries stay sharp and colours are never mixed.
    if (m_useNearestInterpolation)
    {
        imageProperty->SetInterpolationTypeToNearest();
    }
    else
    {
        imageProperty->SetInterpolationTypeToLinear();
    }
    imageProperty->SetOpacity(1.0);

    // Three slice actors, one per axis. Each is driven independently by its
    // checkbox (visibility) and spinbox (slice number) in SeismicVtkViewerDialog.
    // vtkImageSliceMapper's SetOrientationTo{X,Y,Z}() + SetSliceNumber(k) is
    // the canonical way to do an axis-aligned slice on vtkImageData; no
    // vtkPlane indirection is needed.
    const int midIndex[3] = {
        static_cast<int>(trimmedGrid->getNumberOfCellsInX()) / 2,
        static_cast<int>(trimmedGrid->getNumberOfCellsInY()) / 2,
        static_cast<int>(trimmedGrid->getNumberOfCellsInZ()) / 2,
    };
    std::array<vtkSmartPointer<vtkImageSlice>, 3> slices;
    for (int axis = 0; axis < 3; ++axis)
    {
        auto sliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
        sliceMapper->SetInputData(imageData);
        if (axis == 0) sliceMapper->SetOrientationToX();
        else if (axis == 1) sliceMapper->SetOrientationToY();
        else               sliceMapper->SetOrientationToZ();
        sliceMapper->SetSliceNumber(midIndex[axis]);

        auto slice = vtkSmartPointer<vtkImageSlice>::New();
        slice->SetMapper(sliceMapper);
        slice->SetProperty(imageProperty);
        slice->SetScale(1.0, 1.0, M_ZOOM_FACTOR_Z);
        // Start hidden; the dialog shows whichever axes the user ticks.
        slice->SetVisibility(false);

        slices[axis] = slice;
    }

    // m_clipPlane is retained for API compatibility (getClipPlane()) but is
    // no longer the slicing mechanism; vtkImageSliceMapper uses integer
    // slice indices directly.
    m_clipPlane = vtkSmartPointer<vtkPlane>::New();
    m_clipPlane->SetNormal(1.0, 0.0, 0.0);
    m_clipPlane->SetOrigin(0.0, 0.0, 0.0);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    for (auto& s : slices)
    {
        renderer->AddViewProp(s);
    }

    vtkNew<vtkScalarBarActor> scalarBar;
    scalarBar->SetLookupTable(colorTransfer);
    scalarBar->SetTitle(m_scalarBarTitle.toUtf8().constData());
    scalarBar->SetNumberOfLabels(9);
    scalarBar->GetTitleTextProperty()->SetFontSize(14);
    scalarBar->GetLabelTextProperty()->SetFontSize(12);
    scalarBar->SetWidth(0.06);
    scalarBar->SetHeight(0.8);
    scalarBar->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    scalarBar->GetPositionCoordinate()->SetValue(0.92, 0.1);
    scalarBar->SetOrientationToVertical();

    renderer->AddActor2D(scalarBar);
    renderer->SetBackground(0.1, 0.1, 0.1);
    renderer->ResetCamera();

    // Isometric-ish default view so any axis-aligned slice the user picks
    // later is visible instead of edge-on.
    vtkCamera* camera = renderer->GetActiveCamera();
    camera->Elevation(-60.0);
    camera->Azimuth(30.0);
    camera->OrthogonalizeViewUp();
    renderer->ResetCamera();

    m_renderWindow->SetMultiSamples(8);
    m_renderWindow->AddRenderer(renderer);

    m_currentSteps = 99;
    emit stepProgress(m_currentSteps);
}

size_t RegularGridWorker::computeEffectiveDimZ(const domain::RegularGrid<double>& grid)
{
    // Walk backward from k = dimZ-1 and drop XY planes where every cell is
    // either noData or a single constant value (fuzzy compare, since values
    // are doubles — a slice that is nominally uniform may differ in the
    // last few ULPs from processing). The first slice with real variation
    // becomes the bottom of the rendered volume.
    const size_t dimX = grid.getNumberOfCellsInX();
    const size_t dimY = grid.getNumberOfCellsInY();
    const size_t dimZ = grid.getNumberOfCellsInZ();
    const double noDataValue = static_cast<double>(grid.getNoDataValue());

    auto sliceHasVariation = [&](size_t k) -> bool {
        bool refSet = false;
        double refValue = 0.0;
        const double absTol = 1e-9;
        const double relTol = 1e-9;
        for (size_t j = 0; j < dimY; ++j)
        {
            for (size_t i = 0; i < dimX; ++i)
            {
                const double v = static_cast<double>(grid.getData(i, j, k));
                if (v == noDataValue) continue;
                if (!refSet) { refValue = v; refSet = true; continue; }
                const double diff = std::abs(v - refValue);
                const double scale = std::max(std::abs(v), std::abs(refValue));
                if (diff > absTol && diff > relTol * scale) return true;
            }
        }
        return false;
    };

    size_t effectiveDimZ = dimZ;
    while (effectiveDimZ > 1 && !sliceHasVariation(effectiveDimZ - 1))
    {
        --effectiveDimZ;
    }
    return effectiveDimZ;
}

std::shared_ptr<domain::RegularGrid<double>> RegularGridWorker::copyTrimmedZ(
    const domain::RegularGrid<double>& src, size_t newNumberOfCellsInZ)
{
    const size_t dimX = src.getNumberOfCellsInX();
    const size_t dimY = src.getNumberOfCellsInY();

    auto trimmed = std::make_shared<domain::RegularGrid<double>>(
        dimX, dimY, newNumberOfCellsInZ,
        src.getCellSizeInX(), src.getCellSizeInY(), src.getCellSizeInZ(),
        src.getUnitInX(), src.getUnitInY(), src.getUnitInZ(),
        src.getRectanglePoints(),
        src.getZBottom(), src.getZTop(),
        0.0, src.getNoDataValue()
    );
    auto& dst = trimmed->getData();
    for (size_t i = 0; i < dimX; ++i)
    {
        for (size_t j = 0; j < dimY; ++j)
        {
            for (size_t k = 0; k < newNumberOfCellsInZ; ++k)
            {
                dst[i][j][k] = src.getData(i, j, k);
            }
        }
    }
    return trimmed;
}

RegularGridWorker::FillResult RegularGridWorker::fillImageDataFromGrid(
    const domain::RegularGrid<double>& grid, vtkImageData* imageData,
    int progressLo, int progressHi)
{
    const size_t dimX = grid.getNumberOfCellsInX();
    const size_t dimY = grid.getNumberOfCellsInY();
    const size_t dimZ = grid.getNumberOfCellsInZ();
    const double noDataValue = static_cast<double>(grid.getNoDataValue());
    float* const rawScalars = static_cast<float*>(imageData->GetScalarPointer());
    const size_t totalSamples = dimX * dimY * dimZ;

    const int progressSpan = std::max(1, progressHi - progressLo);
    const size_t samplesPerStep = std::max<size_t>(1, totalSamples / static_cast<size_t>(progressSpan));

    std::atomic<size_t> samplesProcessed{0};
    std::atomic<uint32_t> minBits{0x7F800000u}; // +inf
    std::atomic<uint32_t> maxBits{0xFF800000u}; // -inf
    auto atomicMinFloat = [](std::atomic<uint32_t>& target, float value) {
        uint32_t newBits;
        std::memcpy(&newBits, &value, sizeof(newBits));
        uint32_t current = target.load(std::memory_order_relaxed);
        float currentVal;
        do {
            std::memcpy(&currentVal, &current, sizeof(currentVal));
            if (!(value < currentVal)) return;
        } while (!target.compare_exchange_weak(current, newBits, std::memory_order_relaxed));
    };
    auto atomicMaxFloat = [](std::atomic<uint32_t>& target, float value) {
        uint32_t newBits;
        std::memcpy(&newBits, &value, sizeof(newBits));
        uint32_t current = target.load(std::memory_order_relaxed);
        float currentVal;
        do {
            std::memcpy(&currentVal, &current, sizeof(currentVal));
            if (!(value > currentVal)) return;
        } while (!target.compare_exchange_weak(current, newBits, std::memory_order_relaxed));
    };

    const size_t hwConcurrency = std::max<size_t>(1, std::thread::hardware_concurrency());
    const size_t slicesPerThread = std::max<size_t>(1, dimZ / hwConcurrency);

    std::vector<std::future<void>> tasks;
    for (size_t kStart = 0; kStart < dimZ; kStart += slicesPerThread)
    {
        const size_t kEnd = std::min(kStart + slicesPerThread, dimZ);
        tasks.emplace_back(std::async(std::launch::async, [=, &grid, &samplesProcessed, &minBits, &maxBits]() {
            for (size_t k = kStart; k < kEnd; ++k)
            {
                for (size_t j = 0; j < dimY; ++j)
                {
                    for (size_t i = 0; i < dimX; ++i)
                    {
                        const size_t idx = k * dimY * dimX + j * dimX + i;
                        const float value = static_cast<float>(grid.getData(i, j, k));

                        rawScalars[idx] = value;

                        if (static_cast<double>(value) != noDataValue)
                        {
                            atomicMinFloat(minBits, value);
                            atomicMaxFloat(maxBits, value);
                        }

                        const size_t count = ++samplesProcessed;
                        if (count % samplesPerStep == 0 && m_currentSteps < progressHi)
                        {
                            ++m_currentSteps;
                            emit stepProgress(m_currentSteps);
                        }
                    }
                }
            }
        }));
    }
    for (auto& task : tasks)
    {
        task.get();
    }

    m_currentSteps = progressHi;
    emit stepProgress(progressHi);

    float minValueF, maxValueF;
    const uint32_t minB = minBits.load();
    const uint32_t maxB = maxBits.load();
    std::memcpy(&minValueF, &minB, sizeof(minValueF));
    std::memcpy(&maxValueF, &maxB, sizeof(maxValueF));

    double minValue = static_cast<double>(minValueF);
    double maxValue = static_cast<double>(maxValueF);
    if (!std::isfinite(minValue) || !std::isfinite(maxValue))
    {
        minValue = 0.0;
        maxValue = 1.0;
    }
    else if (!(maxValue > minValue))
    {
        maxValue = minValue + 1.0;
    }
    return {minValue, maxValue};
}

}
}
