#include "TestMain.h"

#include <gtest/gtest.h>
#include <QFile>
#include <QtTest/QtTest>
#include <type_traits>

#include <domain/src/ExtractVolumes.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <domain/src/ExtractMinimumRectangle2D.h>
#include <domain/src/RotateVolumeCoordinate.h>
#include <storage/src/reader/EclipseGridReader.h>
#include <storage/src/writer/RegularGridHdf5Writer.h>

class GTestExecutionControl : public ::testing::EmptyTestEventListener
{
public:
    ~GTestExecutionControl() override;

    // Fired before the test case starts.
    void OnTestCaseStart(const ::testing::TestCase &test_case) override;

    // Fired before the test starts.
    void OnTestStart(const ::testing::TestInfo &test_info) override;

    // Fired after a failed assertion or a SUCCEED() invocation.
    void OnTestPartResult(const ::testing::TestPartResult &) override;

    // Fired after the test ends.
    void OnTestEnd(const ::testing::TestInfo &test_info) override;

    // Fired after the test case ends.
    void OnTestCaseEnd(const ::testing::TestCase &test_case) override;
};

GTestExecutionControl::~GTestExecutionControl() {}

void GTestExecutionControl::OnTestCaseStart(const testing::TestCase &)
{
}

void GTestExecutionControl::OnTestStart(const testing::TestInfo &)
{
}

void GTestExecutionControl::OnTestPartResult(const ::testing::TestPartResult &r)
{
    if (r.failed()) {
        QFAIL("GTest failed");
    }
}

void GTestExecutionControl::OnTestEnd(const testing::TestInfo &)
{
}

void GTestExecutionControl::OnTestCaseEnd(const testing::TestCase &)
{
}

void TestMain::runGTest()
{
    QCOMPARE(::testing::UnitTest::GetInstance()->Run(), 0);
}

//#if defined(QT_GUI_LIB) || defined(QT_WIDGETS_LIB)

//QT_BEGIN_NAMESPACE
//QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
//QT_END_NAMESPACE
//int main(int argc, char *argv[])
//{
//    ::testing::InitGoogleTest(&argc, argv);
//    ::testing::UnitTest::GetInstance()->listeners().Append(new GTestExecutionControl());

//    QGuiApplication app(argc, argv);
//    app.setAttribute(Qt::AA_Use96Dpi, true);
//    QTEST_ADD_GPU_BLACKLIST_SUPPORT
//    TestMain tc;
//    QTEST_SET_MAIN_SOURCE_PATH
//    return QTest::qExec(&tc, argc, argv);
//}
//#else

std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> read(QString basePath,
          QString path
          )
{
    using namespace syntheticSeismic::domain;

    QString error;

    std::cout << "read..." << std::endl;
    const syntheticSeismic::storage::EclipseGridReader reader(basePath + "/" + path);
    const auto eclipseGrid = reader.read(error);
    std::cout << "volumesOfFirstLayer..." << std::endl;
    const auto volumesOfFirstLayer = ExtractVolumes::extractFirstLayerFrom(eclipseGrid);

    std::cout << "extractFromVolumesOfFirstLayer..." << std::endl;
    return ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, eclipseGrid, true);
}

int main(int argc, char *argv[])
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;
    using namespace syntheticSeismic::storage;

    RegularGrid<int> *regularGridLithology;
    size_t numberOfCellsInX = 287;
    size_t numberOfCellsInY = 735;
    size_t numberOfCellsInZ = 993;
    QString basePathHdf5 = "D:/gridMolles/";

    {
        QString basePathForRead = "D:/gridMolles/";

        std::vector<std::shared_ptr<Volume>> volumes;

        {
            std::cout << "Processando grid_Molles10:" << std::endl;
            auto volumesGridMolles10 = read(basePathForRead, "grid_Molles10.grdecl");
            std::cout << "Processando grid_Molles20:" << std::endl;
            auto volumesGridMolles20 = read(basePathForRead, "grid_Molles20.grdecl");
            std::cout << "Processando grid_Molles30:" << std::endl;
            auto volumesGridMolles30 = read(basePathForRead, "grid_Molles30.grdecl");
            std::cout << "Processando grid_Molles40:" << std::endl;
            auto volumesGridMolles40 = read(basePathForRead, "grid_Molles40.grdecl");
            std::cout << "Fim processando." << std::endl << std::endl;

            std::cout << "Merge de volumes..." << std::endl;

            volumes.insert(volumes.end(), volumesGridMolles10.begin(), volumesGridMolles10.end());
            volumes.insert(volumes.end(), volumesGridMolles20.begin(), volumesGridMolles20.end());
            volumes.insert(volumes.end(), volumesGridMolles30.begin(), volumesGridMolles30.end());
            volumes.insert(volumes.end(), volumesGridMolles40.begin(), volumesGridMolles40.end());
        }

        std::cout << "Calculando retângulo mínimo..." << std::endl;
        std::cout << std::setprecision(9);
        const auto minimumRectangle = ExtractMinimumRectangle2D::extractFrom(volumes);
        for (const auto &item : minimumRectangle)
        {
            std::cout << "x: " << item.x << " y: " << item.y << std::endl;
        }
        std::cout << "Rotacionando volumes..." << std::endl;
        RotateVolumeCoordinate::rotateByMinimumRectangle(volumes, minimumRectangle);
        std::cout << "Quantidade de volumes: " << volumes.size() << std::endl;

        std::cout << "Fim processamento dos volumes" << std::endl << std::endl;

        std::cout << "Transformando em regular grid..." << std::endl;

        VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
        auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumes);
        const auto &data = regularGrid.getData();

        std::cout << "Fim Transformando em regular grid." << std::endl;

        std::cout << "Transformando em regular grid lithology ..." << std::endl;
        regularGridLithology = new RegularGrid<int>(
                    regularGrid.getNumberOfCellsInX(), regularGrid.getNumberOfCellsInY(), regularGrid.getNumberOfCellsInZ(),
                    regularGrid.getCellSizeInX(), regularGrid.getCellSizeInY(), regularGrid.getCellSizeInZ(),
                    -1,
                    -1
                );
        regularGridLithology->setRectanglePoints(minimumRectangle);

        auto &dataLithology = regularGridLithology->getData();
        for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
        {
            for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
            {
                for (size_t z = 0; z < regularGrid.getNumberOfCellsInZ(); ++z)
                {
                    if (data[x][y][z] == nullptr)
                    {
                        dataLithology[x][y][z] = regularGridLithology->getNoDataValue();
                    }
                    else
                    {
                        dataLithology[x][y][z] = data[x][y][z]->idLithology;
                    }
                }
            }
        }

        std::cout << "Fim transformando em regular grid lithology." << std::endl;
    }

    size_t numberOfParts = 1;
    size_t sectionIncrement = numberOfCellsInZ / numberOfParts;
    for (size_t sectionStart = 0, sectionIndex = 0; sectionStart < numberOfCellsInZ; sectionStart += sectionIncrement, ++sectionIndex)
    {
        std::cout << "Gravando section " << sectionIndex << "..." << std::endl;

        QString fileName = basePathHdf5;
        fileName += "regularGridData_993InZ";
        fileName += QString::number(sectionIndex);
        fileName += ".h5";

        QString datasetName = "grid";
        auto regularGridWriter = RegularGridHdf5Writer<int>(fileName, datasetName);

        auto limitZ = std::min(regularGridLithology->getNumberOfCellsInZ(), sectionStart + sectionIncrement);

        auto numberZ = limitZ - sectionStart;
        RegularGrid<int> regularGridSection(
                    regularGridLithology->getNumberOfCellsInX(), regularGridLithology->getNumberOfCellsInY(), numberZ,
                    regularGridLithology->getCellSizeInX(), regularGridLithology->getCellSizeInY(), regularGridLithology->getCellSizeInZ(),
                    -1,
                    -1
                );

        for (size_t x = 0; x < regularGridLithology->getNumberOfCellsInX(); ++x)
        {
            for (size_t y = 0; y < regularGridLithology->getNumberOfCellsInY(); ++y)
            {
                for (size_t z = sectionStart, zAux = 0; z < limitZ; ++z, ++zAux)
                {
                    regularGridSection.getData()[x][y][zAux] = regularGridLithology->getData()[x][y][z];
                }
            }
        }
        regularGridWriter.write(regularGridSection);
    }

    return 0;
/*
    size_t numberOfParts = 5;
    size_t sectionIncrement = numberOfCellsInZ / numberOfParts;
    for (size_t sectionStart = 0, sectionIndex = 0; sectionStart < numberOfCellsInZ; sectionStart += sectionIncrement, ++sectionIndex)
    {
        std::cout << "Gravando section " << sectionIndex << "..." << std::endl;

        QString fileName = basePath;
        fileName += "regularGridData_";
        fileName += QString::number(sectionIndex);
        fileName += ".py";
        const QString lineBreaker("\n");
        const QString comma(", ");

        QFile fs(fileName);
        if (!fs.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            throw std::invalid_argument("Error openning file!");
        }
        QTextStream out(&fs);
        out << "import numpy as np" << lineBreaker << lineBreaker;
        out << "cellSizeInX = " << regularGrid.getCellSizeInX() << lineBreaker;
        out << "cellSizeInY = " << regularGrid.getCellSizeInY() << lineBreaker;
        out << "cellSizeInZ = " << regularGrid.getCellSizeInZ() << lineBreaker;
        out << lineBreaker;

        out << "values = np.array(" << lineBreaker;
        out << "\t[" << lineBreaker;
        auto limitZ = std::min(regularGrid.getNumberOfCellsInZ(), sectionStart + sectionIncrement);
        for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
        {
            out << "\t\t[" << lineBreaker;
            for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
            {
                out << "\t\t\t[";
                for (size_t z = sectionStart; z < limitZ; ++z)
                {
                    if (data[x][y][z] == nullptr)
                    {
                        out << "-1";
                    }
                    else
                    {
                        out << data[x][y][z]->idLithology;
                    }
                    out << comma;
                }
                out << "]" << comma << lineBreaker;
            }
            out << "\t\t]" << comma << lineBreaker;
        }
        out << "\t]" << lineBreaker;
        out << ")";
    }

    return 0;
*/



    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new GTestExecutionControl());

    QCoreApplication app(argc, argv);

//    const QString testFolder = QString("%1/test_folder").arg(app.applicationDirPath());
//    QDir dir;

//    dir.rmpath(testFolder);
//    dir.mkpath(testFolder);
//    dir.cd(testFolder);

    app.setAttribute(Qt::AA_Use96Dpi, true);
    TestMain tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

//#endif

