#include "RegularGridWorker.h"

#include <QTime>
#include <QElapsedTimer>
#include <QVTKOpenGLNativeWidget.h>

#include <future>
#include <mutex>

#include <vtkRenderer.h>
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


namespace syntheticSeismic {
namespace widgets {

RegularGridWorker::RegularGridWorker(const std::shared_ptr<domain::RegularGrid<double>>& regularGrid) :
    m_regularGrid(regularGrid)
{
}

RegularGridWorker::~RegularGridWorker()
{
}

void RegularGridWorker::run()
{
    QElapsedTimer timer;
    timer.start();
    std::cout << "** buildGrid() started: " << std::endl;

    buildGrid();

    const qint64& elapsedMs = timer.elapsed();

    QTime time = QTime(0, 0).addMSecs(static_cast<int>(elapsedMs));
    QString formattedTime = time.toString("hh:mm:ss");
    std::cout << "** buildGrid() finished: " << formattedTime.toStdString() << std::endl;

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

    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints(totalPoints);

    auto scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->SetName("Amplitude");
    scalars->SetNumberOfValues(totalPoints);

    std::mutex pointMutex;
    std::mutex scalarMutex;

    std::vector<std::future<void>> tasks;

    const size_t slicesPerThread = std::max<size_t>(1, dimZ / std::thread::hardware_concurrency());
    std::atomic<size_t> pointsProcessed{0};
    const size_t volumesPerStep = std::max<size_t>(1, totalPoints / 80);

    for (size_t kStart = 0; kStart < dimZ; kStart += slicesPerThread)
    {
        const size_t kEnd = std::min(kStart + slicesPerThread, dimZ);

        tasks.emplace_back(std::async(std::launch::async, [=, &points, &scalars, &pointsProcessed]() {
            for (size_t k = kStart; k < kEnd; ++k)
            {
                for (size_t j = 0; j < dimY; ++j)
                {
                    for (size_t i = 0; i < dimX; ++i)
                    {
                        const vtkIdType idx = k * dimY * dimX + j * dimX + i;
                        double x = i;
                        double y = j;
                        double z = k;
                        float value = static_cast<float>(m_regularGrid->getData(i, j, k));

                        points->SetPoint(idx, x, y, z);
                        scalars->SetValue(idx, value);

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

    double range[2];
    scalars->GetRange(range);

    const double& minValue = -1.0;//range[0];
    const double& maxValue = 1.0;//range[1];

    const double& midValue = (minValue + maxValue) / 2.0;
    const int& numIntermediateSteps = 8;

    vtkNew<vtkColorTransferFunction> colorTransfer;
    colorTransfer->SetColorSpaceToRGB();

    for (int i = 0; i <= numIntermediateSteps; ++i) {
        double t = static_cast<double>(i) / numIntermediateSteps;
        double val = minValue + t * (midValue - minValue);
        double r = t;
        double g = t;
        double b = 1.0;

        colorTransfer->AddRGBPoint(val, r, g, b);
    }

    for (int i = 1; i <= numIntermediateSteps; ++i) { // começa em 1 para não repetir midValue
        double t = static_cast<double>(i) / numIntermediateSteps;
        double val = midValue + t * (maxValue - midValue);
        double r = 1.0;
        double g = 1.0 - t;
        double b = 1.0 - t;

        colorTransfer->AddRGBPoint(val, r, g, b);
    }

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(grid);
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
    scalarBar->SetTitle("Amp");
    scalarBar->SetNumberOfLabels(9);
    scalarBar->GetTitleTextProperty()->SetFontSize(14);
    scalarBar->GetLabelTextProperty()->SetFontSize(12);
    scalarBar->SetMaximumWidthInPixels(100);
    scalarBar->SetPosition(0.88, 0.1);
    scalarBar->SetWidth(0.1);
    scalarBar->SetHeight(0.8);

    renderer->AddActor2D(scalarBar);

    renderer->ResetCamera();
    m_renderWindow->AddRenderer(renderer);

    m_currentSteps = 99;
    emit stepProgress(m_currentSteps);
}

}
}

