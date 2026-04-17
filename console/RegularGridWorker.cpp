#include "RegularGridWorker.h"

#include <QTime>
#include <QElapsedTimer>
#include <QDebug>
#include <QVTKOpenGLNativeWidget.h>

#include <future>
#include <cstdint>
#include <cstring>

#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkScalarBarActor.h>
#include <vtkStructuredGrid.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPlane.h>
#include <vtkClipDataSet.h>


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
    const size_t totalPoints = dimX * dimY * dimZ;
    const double noDataValue = static_cast<double>(m_regularGrid->getNoDataValue());

    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints(totalPoints);

    auto scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->SetName(m_scalarBarTitle.toUtf8().constData());
    scalars->SetNumberOfValues(totalPoints);

    std::vector<std::future<void>> tasks;

    const size_t hwConcurrency = std::max<size_t>(1, std::thread::hardware_concurrency());
    const size_t slicesPerThread = std::max<size_t>(1, dimZ / hwConcurrency);
    std::atomic<size_t> pointsProcessed{0};
    const size_t volumesPerStep = std::max<size_t>(1, totalPoints / 80);

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

        tasks.emplace_back(std::async(std::launch::async, [=, &points, &scalars, &pointsProcessed, &minBits, &maxBits]() {
            for (size_t k = kStart; k < kEnd; ++k)
            {
                for (size_t j = 0; j < dimY; ++j)
                {
                    for (size_t i = 0; i < dimX; ++i)
                    {
                        const vtkIdType idx = static_cast<vtkIdType>(k * dimY * dimX + j * dimX + i);
                        const double x = static_cast<double>(i);
                        const double y = static_cast<double>(j);
                        // Negate Z so depth (increasing k) goes downward in world space,
                        // matching the EclipseGridWorker convention expected by VtkViewerDialog slicing.
                        const double z = -static_cast<double>(k);
                        const float value = static_cast<float>(m_regularGrid->getData(i, j, k));

                        points->SetPoint(idx, x, y, z);
                        scalars->SetValue(idx, value);

                        if (static_cast<double>(value) != noDataValue)
                        {
                            atomicMinFloat(minBits, value);
                            atomicMaxFloat(maxBits, value);
                        }

                        const size_t count = ++pointsProcessed;
                        if (count % volumesPerStep == 0 && m_currentSteps < 98) {
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

    vtkNew<vtkStructuredGrid> grid;
    grid->SetDimensions(static_cast<int>(dimX), static_cast<int>(dimY), static_cast<int>(dimZ));
    grid->SetPoints(points);
    grid->GetPointData()->SetScalars(scalars);

    float minValueF, maxValueF;
    const uint32_t minB = minBits.load();
    const uint32_t maxB = maxBits.load();
    std::memcpy(&minValueF, &minB, sizeof(minValueF));
    std::memcpy(&maxValueF, &maxB, sizeof(maxValueF));

    double minValue = static_cast<double>(minValueF);
    double maxValue = static_cast<double>(maxValueF);
    if (!(maxValue > minValue))
    {
        // Fallback if grid has no valid data or is flat.
        double range[2];
        scalars->GetRange(range);
        minValue = range[0];
        maxValue = range[1];
        if (!(maxValue > minValue))
        {
            maxValue = minValue + 1.0;
        }
    }

    const double midValue = (minValue + maxValue) / 2.0;
    const int numIntermediateSteps = 8;

    vtkNew<vtkColorTransferFunction> colorTransfer;
    colorTransfer->SetColorSpaceToRGB();

    // Blue -> white (min -> mid)
    for (int i = 0; i <= numIntermediateSteps; ++i) {
        const double t = static_cast<double>(i) / numIntermediateSteps;
        const double val = minValue + t * (midValue - minValue);
        colorTransfer->AddRGBPoint(val, t, t, 1.0);
    }

    // White -> red (mid -> max); start at i=1 so we don't re-declare midValue.
    for (int i = 1; i <= numIntermediateSteps; ++i) {
        const double t = static_cast<double>(i) / numIntermediateSteps;
        const double val = midValue + t * (maxValue - midValue);
        colorTransfer->AddRGBPoint(val, 1.0, 1.0 - t, 1.0 - t);
    }

    // vtkClipDataSet between grid and mapper: when the dialog moves the plane,
    // the filter regenerates geometry at the cut face so the interior (colored
    // by scalars) becomes visible. Default plane sits far outside the data
    // bounds so that F(point) = (point - origin) . normal > 0 for every cell,
    // which VTK's default (InsideOut off) keeps — i.e., the full volume is
    // shown until the dialog starts slicing.
    m_clipPlane = vtkSmartPointer<vtkPlane>::New();
    m_clipPlane->SetNormal(-1.0, 0.0, 0.0);
    m_clipPlane->SetOrigin(-1.0, 0.0, 0.0);

    auto clipFilter = vtkSmartPointer<vtkClipDataSet>::New();
    clipFilter->SetInputData(grid);
    clipFilter->SetClipFunction(m_clipPlane);

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(clipFilter->GetOutputPort());
    mapper->SetScalarRange(minValue, maxValue);
    mapper->SetLookupTable(colorTransfer);
    mapper->ScalarVisibilityOn();
    mapper->SetColorModeToMapScalars();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(1.0);
    actor->GetProperty()->EdgeVisibilityOff();
    actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetRepresentationToSurface();
    actor->SetScale(1.0, 1.0, M_ZOOM_FACTOR_Z);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

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
    renderer->SetAmbient(0.9, 0.9, 0.9);
    renderer->ResetCamera();

    // Preset view: look down the -Z axis (top-down) like the Eclipse viewer.
    vtkCamera* camera = renderer->GetActiveCamera();
    camera->Elevation(-90.0);
    camera->Azimuth(0.0);
    camera->OrthogonalizeViewUp();
    renderer->ResetCamera();

    m_renderWindow->SetMultiSamples(8); // anti-aliasing
    m_renderWindow->AddRenderer(renderer);

    m_currentSteps = 99;
    emit stepProgress(m_currentSteps);
}

}
}
