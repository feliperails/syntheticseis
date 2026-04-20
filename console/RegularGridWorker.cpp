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
                                     const QString& scalarBarTitle) :
    m_regularGrid(regularGrid),
    m_scalarBarTitle(scalarBarTitle)
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

    const size_t dimX = m_regularGrid->getNumberOfCellsInX();
    const size_t dimY = m_regularGrid->getNumberOfCellsInY();
    const size_t dimZ = m_regularGrid->getNumberOfCellsInZ();
    const double noDataValue = static_cast<double>(m_regularGrid->getNoDataValue());

    // Trim trailing empty X slices: walk backward from i = dimX-1 and drop
    // YZ planes where every cell is either noData or a single constant value
    // (fuzzy compare — the grid stores doubles, so a slice that is nominally
    // uniform may differ in the last few ULPs from processing). The first
    // slice with real variation becomes the right edge of the rendered
    // volume, so a large no-data block at the end of X does not dominate
    // the rendered extent.
    auto sliceHasVariation = [&](size_t i) -> bool {
        bool refSet = false;
        double refValue = 0.0;
        const double absTol = 1e-9;
        const double relTol = 1e-9;
        for (size_t k = 0; k < dimZ; ++k)
        {
            for (size_t j = 0; j < dimY; ++j)
            {
                const double v = static_cast<double>(m_regularGrid->getData(i, j, k));
                if (v == noDataValue) continue;
                if (!refSet) { refValue = v; refSet = true; continue; }
                const double diff = std::abs(v - refValue);
                const double scale = std::max(std::abs(v), std::abs(refValue));
                if (diff > absTol && diff > relTol * scale) return true;
            }
        }
        return false;
    };

    size_t effectiveDimX = dimX;
    while (effectiveDimX > 1 && !sliceHasVariation(effectiveDimX - 1))
    {
        --effectiveDimX;
    }
    if (effectiveDimX != dimX)
    {
        qInfo().noquote() << "RegularGridWorker: trimmed" << (dimX - effectiveDimX)
                          << "trailing empty X slices (kept" << effectiveDimX << "of" << dimX << ").";
    }

    const size_t totalSamples = effectiveDimX * dimY * dimZ;

    // Seismic viewer pattern borrowed from CimaView3D (cs2i-senai-cimatec):
    // the data is rendered as a set of axis-aligned slice planes textured
    // with the amplitudes on that plane, not as a 3D volume. In VTK this is
    // vtkImageResliceMapper + vtkImageSlice. Three planes, one per axis,
    // positioned by the dialog's slider. Every pixel on a slice face IS the
    // amplitude at that (x, y, z) point, so interior cells are visible by
    // construction — unlike surface rendering, which only draws the hull.
    auto imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->SetDimensions(static_cast<int>(effectiveDimX), static_cast<int>(dimY), static_cast<int>(dimZ));
    imageData->SetOrigin(0.0, 0.0, 0.0);
    // Z spacing negated so depth (increasing k) goes downward in world space,
    // matching the EclipseGridWorker convention expected by VtkViewerDialog slicing.
    imageData->SetSpacing(1.0, 1.0, -1.0);
    imageData->AllocateScalars(VTK_FLOAT, 1);
    float* const rawScalars = static_cast<float*>(imageData->GetScalarPointer());

    std::vector<std::future<void>> tasks;

    const size_t hwConcurrency = std::max<size_t>(1, std::thread::hardware_concurrency());
    const size_t slicesPerThread = std::max<size_t>(1, dimZ / hwConcurrency);
    std::atomic<size_t> samplesProcessed{0};
    const size_t samplesPerStep = std::max<size_t>(1, totalSamples / 80);

    // Data range over valid (non-noData) values, computed in parallel.
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

    for (size_t kStart = 0; kStart < dimZ; kStart += slicesPerThread)
    {
        const size_t kEnd = std::min(kStart + slicesPerThread, dimZ);

        tasks.emplace_back(std::async(std::launch::async, [=, &samplesProcessed, &minBits, &maxBits]() {
            for (size_t k = kStart; k < kEnd; ++k)
            {
                for (size_t j = 0; j < dimY; ++j)
                {
                    for (size_t i = 0; i < effectiveDimX; ++i)
                    {
                        const size_t idx = k * dimY * effectiveDimX + j * effectiveDimX + i;
                        const float value = static_cast<float>(m_regularGrid->getData(i, j, k));

                        rawScalars[idx] = value;

                        if (static_cast<double>(value) != noDataValue)
                        {
                            atomicMinFloat(minBits, value);
                            atomicMaxFloat(maxBits, value);
                        }

                        const size_t count = ++samplesProcessed;
                        if (count % samplesPerStep == 0 && m_currentSteps < 98) {
                            ++m_currentSteps;
                            emit stepProgress(m_currentSteps);
                        }
                    }
                }
            }
        }));
    }

    size_t tasksProcessed{0};
    const size_t tasksPerStep = std::max<size_t>(1, tasks.size() / 18);
    for (auto& task : tasks)
    {
        task.get();

        const size_t count = ++tasksProcessed;
        if (count % tasksPerStep == 0 && m_currentSteps < 98) {
            ++m_currentSteps;
            emit stepProgress(m_currentSteps);
        }
    }

    m_currentSteps = 98;
    emit stepProgress(m_currentSteps);

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
    imageProperty->SetInterpolationTypeToLinear();
    imageProperty->SetOpacity(1.0);

    // Three slice actors, one per axis. Each is driven independently by its
    // checkbox (visibility) and spinbox (slice number) in SeismicVtkViewerDialog.
    // vtkImageSliceMapper's SetOrientationTo{X,Y,Z}() + SetSliceNumber(k) is
    // the canonical way to do an axis-aligned slice on vtkImageData; no
    // vtkPlane indirection is needed.
    const int midIndex[3] = {
        static_cast<int>(effectiveDimX) / 2,
        static_cast<int>(dimY) / 2,
        static_cast<int>(dimZ) / 2,
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

}
}
