#include "EclipseGridWorker.h"
#include "geometry/src/Point3D.h"

#include <QTime>
#include <QElapsedTimer>
#include <QVTKOpenGLNativeWidget.h>

#include <unordered_map>
#include <vector>
#include <array>
#include <future>
#include <mutex>

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
    // ANP colors
    m_defaultColors = {
        { {0.400, 0.760, 0.647} },  // 1 - Arenito
        { {0.992, 0.705, 0.384} },  // 2 - Siltito
        { {0.541, 0.584, 0.741} },  // 3 - Argilito
        { {0.745, 0.729, 0.855} },  // 4 - Folhelho
        { {0.867, 0.867, 0.867} },  // 5 - Calcário
        { {0.984, 0.603, 0.600} },  // 6 - Dolomito
        { {0.552, 0.627, 0.796} },  // 7 - Conglomerado
        { {0.851, 0.851, 0.851} },  // 8 - Brecha
        { {0.737, 0.741, 0.133} },  // 9 - Evaporito
        { {0.698, 0.874, 0.541} },  //10 - Sal

        { {0.133, 0.133, 0.600} },  //11 - Diabásio
        { {0.733, 0.078, 0.102} },  //12 - Basalto
        { {0.984, 0.603, 0.200} },  //13 - Chert (Sílex)
        { {0.847, 0.372, 0.007} },  //14 - Carvão
        { {0.000, 0.447, 0.698} },  //15 - Tufo
        { {0.329, 0.784, 0.494} },  //16 - Arenito calcário
        { {0.749, 0.356, 0.090} },  //17 - Folhelho calcífero
        { {0.800, 0.922, 0.772} },  //18 - Argila
        { {0.580, 0.404, 0.741} },  //19 - Gnaisse
        { {0.968, 0.714, 0.824} },  //20 - Xisto

        { {0.302, 0.686, 0.290} },  //21 - Marga
        { {0.596, 0.306, 0.639} },  //22 - Arenito dolomítico
        { {1.000, 0.498, 0.000} },  //23 - Calcarenito
        { {1.000, 1.000, 0.200} },  //24 - Micrito
        { {0.651, 0.337, 0.157} },  //25 - Siltito calcário
        { {0.969, 0.506, 0.749} },  //26 - Diatomito
        { {0.600, 0.600, 0.600} },  //27 - Rochas metamórficas diversas
        { {0.200, 0.200, 0.200} },  //28 - Rochas ígneas diversas
        { {0.894, 0.102, 0.110} },  //29 - Indefinida
        { {0.216, 0.494, 0.722} }   //30 - Outra litologia
    };

}

void EclipseGridWorker::run()
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

    static const int vtkToVolumeIndex[8] = { 0, 4, 6, 2, 1, 5, 7, 3 };

    std::vector<std::future<void>> tasks;

    m_currentSteps = 1;
    emit stepProgress(m_currentSteps);

    std::atomic<size_t> volumesProcessed{0};
    const size_t volumesPerStep = std::max<size_t>(1, m_allVolumes->size() / 80);

    for (const auto& volume : (*m_allVolumes)) {
        tasks.emplace_back(std::async(std::launch::async, [&]() {


            if (!volume->actnum) return;

            auto hex = vtkSmartPointer<vtkHexahedron>::New();

            for (int i = 0; i < 8; ++i) {
                const auto& pt = volume->points[vtkToVolumeIndex[i]];

                vtkIdType id;
                const size_t& key = hashPoint(pt);

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
            if (count % volumesPerStep == 0 && m_currentSteps < 98) {
                ++m_currentSteps;
                emit stepProgress(m_currentSteps);
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

    grid->SetPoints(points);
    grid->GetCellData()->SetScalars(lithologyArray);

    double range[2];
    lithologyArray->GetRange(range);

    const int& minId = static_cast<int>(range[0]);
    const int& maxId = static_cast<int>(range[1]);
    const int& numColors = maxId - minId + 1;

    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(numColors);
    lut->SetRange(minId, maxId);
    lut->Build();

    for (int i = 0; i < numColors; ++i)
    {
        const size_t& colorIndex = i % m_defaultColors.size();

        const double& r = m_defaultColors[colorIndex][0];
        const double& g = m_defaultColors[colorIndex][1];
        const double& b = m_defaultColors[colorIndex][2];
        lut->SetTableValue(colorIndex, r, g, b);
    }


    auto m_transform = vtkSmartPointer<vtkTransform>::New();

    auto m_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    m_transformFilter->SetInputData(grid);
    m_transformFilter->SetTransform(m_transform);
    m_transformFilter->Update();

    auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(grid);
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(lithologyArray->GetRange());
    mapper->SetInputConnection(m_transformFilter->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(1.0);
    actor->GetProperty()->EdgeVisibilityOff();
    actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetRepresentationToSurface();
    actor->SetScale(1.0, 1.0, M_ZOOM_FACTOR_Z);

    auto scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(lut);
    scalarBar->SetTitle("Litho");
    scalarBar->SetNumberOfLabels(numColors);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();

    renderer->AddActor(actor);
    renderer->AddActor2D(scalarBar);
    renderer->SetAmbient(0.9, 0.9, 0.9);
    renderer->SetBackground(0.1, 0.1, 0.1);
    renderer->ResetCamera();

    m_renderWindow->SetMultiSamples(8); // anti-aliasing
    m_renderWindow->AddRenderer(renderer);

    m_currentSteps = 99;
    emit stepProgress(m_currentSteps);
}

}
}

