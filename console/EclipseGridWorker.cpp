#include "EclipseGridWorker.h"

#include <unordered_map>
#include <vector>
#include <array>
#include <future>
#include <mutex>

#include "geometry/src/Point3D.h"

#include <QVTKOpenGLNativeWidget.h>

#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkHexahedron.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkNamedColors.h>

#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkLight.h>
#include <vtkSmartPointer.h>

#include <QElapsedTimer>
#include <QTime>

namespace syntheticSeismic {
namespace widgets {

EclipseGridWorker::EclipseGridWorker(const std::vector<std::shared_ptr<Volume>> * const & allVolumes) :
    m_allVolumes(allVolumes)
{
    init();
}

EclipseGridWorker::~EclipseGridWorker()
{
}

void EclipseGridWorker::init()
{
    m_defaultColors = {
        { {0.894, 0.102, 0.110} },
        { {0.216, 0.494, 0.722} },
        { {0.302, 0.686, 0.290} },
        { {0.596, 0.306, 0.639} },
        { {1.000, 0.498, 0.000} },
        { {1.000, 1.000, 0.200} },
        { {0.651, 0.337, 0.157} },
        { {0.969, 0.506, 0.749} },
        { {0.600, 0.600, 0.600} },
        { {0.200, 0.200, 0.200} },

        { {0.400, 0.760, 0.647} },
        { {0.992, 0.705, 0.384} },
        { {0.541, 0.584, 0.741} },
        { {0.745, 0.729, 0.855} },
        { {0.867, 0.867, 0.867} },
        { {0.984, 0.603, 0.600} },
        { {0.552, 0.627, 0.796} },
        { {0.851, 0.851, 0.851} },
        { {0.737, 0.741, 0.133} },
        { {0.698, 0.874, 0.541} },

        { {0.133, 0.133, 0.600} },
        { {0.733, 0.078, 0.102} },
        { {0.984, 0.603, 0.200} },
        { {0.847, 0.372, 0.007} },
        { {0.000, 0.447, 0.698} },
        { {0.329, 0.784, 0.494} },
        { {0.749, 0.356, 0.090} },
        { {0.800, 0.922, 0.772} },
        { {0.580, 0.404, 0.741} },
        { {0.968, 0.714, 0.824} }
    };

}

void EclipseGridWorker::run()
{
    QElapsedTimer timer;
    timer.start();
    std::cout << "** buildGrid() started: " << std::endl;

    buildGrid();

    size_t elapsedMs = timer.elapsed();

    QTime time = QTime(0, 0).addMSecs(elapsedMs);
    QString formattedTime = time.toString("hh:mm:ss");
    std::cout << "** buildGrid() finished: " << formattedTime.toStdString() << std::endl;

    emit finished();
}

void EclipseGridWorker::buildGrid()
{
    m_currentSteps = 0;
    emit stepProgress(m_currentSteps);

    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

    auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    auto points = vtkSmartPointer<vtkPoints>::New();
    points->Allocate(static_cast<vtkIdType>(m_allVolumes->size() * 8));
    grid->Allocate(m_allVolumes->size());

    std::unordered_map<size_t, vtkIdType> pointMap;

    std::mutex gridMutex;
    std::mutex pointMapMutex;

    auto lithologyArray = vtkSmartPointer<vtkFloatArray>::New();
    lithologyArray->SetName("Lithology");
    lithologyArray->SetNumberOfComponents(1);

    auto hashPoint = [](const geometry::Point3D& pt) -> size_t {
        return std::hash<double>{}(pt.x) ^ std::hash<double>{}(pt.y) << 1 ^ std::hash<double>{}(pt.z) << 2;
    };

    std::vector<std::future<void>> tasks;

    m_currentSteps = 1;
    emit stepProgress(m_currentSteps);

    std::atomic<size_t> volumesProcessed{0};
    const size_t volumesPerStep = std::max<size_t>(1, m_allVolumes->size() / 90);

    for (const auto& volume : (*m_allVolumes)) {
        tasks.emplace_back(std::async(std::launch::async, [&]() {


            if (!volume->actnum) return;

            auto hex = vtkSmartPointer<vtkHexahedron>::New();

            for (int i = 0; i < 8; ++i) {
                const auto& pt = volume->points[i];
                size_t key = hashPoint(pt);
                vtkIdType id;

                {
                    std::lock_guard<std::mutex> lock(pointMapMutex);
                    auto it = pointMap.find(key);
                    if (it == pointMap.end()) {
                        id = points->InsertNextPoint(pt.x, pt.y, pt.z);
                        pointMap[key] = id;
                    } else {
                        id = it->second;
                    }
                }

                hex->GetPointIds()->SetId(i, id);
            }

            {
                std::lock_guard<std::mutex> lock(gridMutex);
                grid->InsertNextCell(hex->GetCellType(), hex->GetPointIds());
                lithologyArray->InsertNextValue(static_cast<float>(volume->idLithology));
            }

            const size_t count = ++volumesProcessed;
            if (count % volumesPerStep == 0 && m_currentSteps < 90) {
                int newStep = 1 + (count / volumesPerStep);
                if (newStep > m_currentSteps) {
                    m_currentSteps = newStep;
                    emit stepProgress(m_currentSteps);
                }
            }
        }));
    }

    for (auto& task : tasks) task.get();

    m_currentSteps = 95;
    emit stepProgress(m_currentSteps);

    grid->SetPoints(points);
    grid->GetCellData()->SetScalars(lithologyArray);

    m_currentSteps = 96;
    emit stepProgress(m_currentSteps);

    double range[2];
    lithologyArray->GetRange(range);

    int minId = static_cast<int>(range[0]);
    int maxId = static_cast<int>(range[1]);

    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(maxId - minId + 2);
    lut->SetRange(minId, maxId);
    lut->Build();

    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    for (int i = minId; i <= maxId; ++i)
    {
        const double r = m_defaultColors[i][0];
        const double g = m_defaultColors[i][1];
        const double b = m_defaultColors[i][2];
        lut->SetTableValue(i, r, g, b);
    }


    m_currentSteps = 97;
    emit stepProgress(m_currentSteps);

    double zoomFactor = 5.0;
    m_transform = vtkSmartPointer<vtkTransform>::New();

    m_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    m_transformFilter->SetInputData(grid);
    m_transformFilter->SetTransform(m_transform);
    m_transformFilter->Update();

    m_currentSteps = 98;
    emit stepProgress(m_currentSteps);

    auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(grid);
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(lithologyArray->GetRange());
    mapper->SetInputConnection(m_transformFilter->GetOutputPort());

    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(mapper);
    m_actor->GetProperty()->EdgeVisibilityOn();
    m_actor->GetProperty()->SetOpacity(0.8);
    m_actor->SetScale(1.0, 1.0, zoomFactor);

    auto scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(lut);
    scalarBar->SetTitle("Lithology");
    scalarBar->SetNumberOfLabels(maxId - minId + 1);

    m_currentSteps = 99;
    emit stepProgress(m_currentSteps);

    m_renderer = vtkSmartPointer<vtkRenderer>::New();

    m_renderer->AddActor(m_actor);
    m_renderer->AddActor2D(scalarBar);

    m_renderer->SetAmbient(0.9, 0.9, 0.9);


    m_renderer->SetBackground(0.1, 0.1, 0.1);
    m_renderer->ResetCamera();

    m_renderWindow->AddRenderer(m_renderer);

    m_currentSteps = 100;
    emit stepProgress(m_currentSteps);
}

}
}

