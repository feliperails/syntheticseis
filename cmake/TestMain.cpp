#include "TestMain.h"

#include <gtest/gtest.h>
#include <QFile>
#include <QtTest/QtTest>
#include <type_traits>

#include <domain/src/ConvolutionRegularGridCalculator.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/ExtractMinimumRectangle2D.h>
#include <domain/src/Lithology.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <domain/src/ReflectivityRegularGridCalculator.h>
#include <domain/src/RickerWaveletCalculator.h>
#include <domain/src/RotateVolumeCoordinate.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <geometry/src/Point2D.h>
#include <storage/src/reader/EclipseGridReader.h>
#include <storage/src/RegularGridHdf5Storage.h>

class GTestExecutionControl : public ::testing::EmptyTestEventListener
{
public:
    virtual ~GTestExecutionControl();
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

std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> read(QString path)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    QString error;

    std::cout << "read...\n" << std::flush;
    const syntheticSeismic::storage::EclipseGridReader reader(path);
    const auto eclipseGrid = reader.read(error);
    std::cout << "volumesOfFirstLayer...\n" << std::flush;
    const auto volumesOfFirstLayer = ExtractVolumes::extractFirstLayerFrom(eclipseGrid);

    std::cout << "extractFromVolumesOfFirstLayer...\n" << std::flush;
    return ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, eclipseGrid, true);
}

int main(int argc, char *argv[])
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;
    using namespace syntheticSeismic::storage;

    if (1)
    {
        std::vector<double> frequencies = {10.0, 25.0, 40.0, 75.0};
        double step = 0.46;

        RickerWaveletCalculator rickerWaveletCalculator;
        rickerWaveletCalculator.setFrequency(25.0);
        rickerWaveletCalculator.setStep(step);
        const auto wavelet = rickerWaveletCalculator.extract();
        const auto waveletSize = wavelet->getTraces().size();

        /*
        size_t numberOfCellsInX = 287;
        size_t numberOfCellsInY = 1536;
        size_t numberOfCellsInZ = 2048;
        // size_t numberOfCellsInZ = 2048 - waveletSize + 1;
        QString gridLithologiesPath = "D:/gridMolles/gridMollesLithologies_287_1536_2048.h5";
        QString gridImpedancePath = "D:/gridMolles/gridMollesImpedance_287_1536_2048.h5";
        QString gridReflectivityPath = "D:/gridMolles/gridMollesReflectivity_287_1536_2048.h5";
        QString gridConvolutionPath = "D:/gridMolles/gridMollesConvolution25hz_287_1536_2048.h5";
        */

        size_t numberOfCellsInX = 287;
        size_t numberOfCellsInY = 735;
        // size_t numberOfCellsInZ = 1252 - waveletSize + 1;
        size_t numberOfCellsInZ = 1252;
        std::cout << "numberOfCellsInZ: " << numberOfCellsInZ << std::endl;

        bool saveGridLithologies = true;
        bool saveGridImpedance = false;
        bool saveGridReflectivity = false;

        QString gridLithologiesPath  = "D:/gridMolles/regularGrid/gridMollesSequences_287_735_1252.h5";
        QString gridImpedancePath    = "D:/gridMolles/regularGrid/gridMollesImpedance_287_735_1252.h5";
        QString gridReflectivityPath = "D:/gridMolles/regularGrid/gridMollesReflectivity_287_735_1252.h5";

        QString gridMolles10Path = "D:/gridMolles/EclipseGrid/grid_Molles10.grdecl";
        QString gridMolles20Path = "D:/gridMolles/EclipseGrid/grid_Molles20.grdecl";
        QString gridMolles30Path = "D:/gridMolles/EclipseGrid/grid_Molles30.grdecl";
        QString gridMolles40Path = "D:/gridMolles/EclipseGrid/grid_Molles40.grdecl";
        QString gridLithologiesDatasetName = "data";
        QString gridImpedanceDatasetName = "data";
        QString gridReflectivityDatasetName = "data";
        QString gridConvolutionDatasetName = "data";

        std::shared_ptr<RegularGrid<double>> impedanceRegularGrid;
        std::array<Point2D, 4> minimumRectangle;
        std::shared_ptr<RotateVolumeCoordinate::Result> rotateByMinimumRectangleResult;
        if (1)
        {
            std::vector<std::shared_ptr<Volume>> volumes;
            {
                std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesGridMolles10;
                std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesGridMolles20;
                std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesGridMolles30;
                std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesGridMolles40;

                std::cout << "Processando leitura dos arquivos..." << std::endl;

                #pragma omp parallel sections
                {
                    #pragma omp section
                    {
                        std::cout << "Processando grid_Molles10...\n" << std::flush;
                        volumesGridMolles10 = read(gridMolles10Path);
                        std::cout << "Fim Processando grid_Molles10.\n" << std::flush;
                        for (auto &volume : volumesGridMolles10)
                        {
                            if (volume->idLithology >= 1)
                            {
                                volume->idLithology = 1;
                            }
                        }
                    }
                    #pragma omp section
                    {
                        std::cout << "Processando grid_Molles20...\n" << std::flush;
                        volumesGridMolles20 = read(gridMolles20Path);
                        std::cout << "Fim Processando grid_Molles20.\n" << std::flush;
                        for (auto &volume : volumesGridMolles20)
                        {
                            if (volume->idLithology >= 1)
                            {
                                volume->idLithology = 2;
                            }
                        }
                    }
                    #pragma omp section
                    {
                        std::cout << "Processando grid_Molles30...\n" << std::flush;
                        volumesGridMolles30 = read(gridMolles30Path);
                        std::cout << "Fim Processando grid_Molles30.\n" << std::flush;
                        for (auto &volume : volumesGridMolles30)
                        {
                            if (volume->idLithology >= 1)
                            {
                                volume->idLithology = 3;
                            }
                        }
                    }
                    #pragma omp section
                    {
                        std::cout << "Processando grid_Molles40...\n" << std::flush;
                        volumesGridMolles40 = read(gridMolles40Path);
                        std::cout << "Fim Processando grid_Molles40.\n" << std::flush;
                        for (auto &volume : volumesGridMolles40)
                        {
                            if (volume->idLithology >= 1)
                            {
                                volume->idLithology = 4;
                            }
                        }
                    }
                }

                std::cout << "Fim processando leitura dos arquivos." << std::endl << std::endl;

                std::cout << "Merge de volumes..." << std::endl;

                volumes.insert(volumes.end(), volumesGridMolles10.begin(), volumesGridMolles10.end());
                volumes.insert(volumes.end(), volumesGridMolles20.begin(), volumesGridMolles20.end());
                volumes.insert(volumes.end(), volumesGridMolles30.begin(), volumesGridMolles30.end());
                volumes.insert(volumes.end(), volumesGridMolles40.begin(), volumesGridMolles40.end());
            }

            std::cout << "Calculando retângulo mínimo..." << std::endl;
            minimumRectangle = extractMinimumRectangle2D::extractFrom(volumes);
            for (const auto &item : minimumRectangle)
            {
                std::cout << "x: " << item.x << " y: " << item.y << std::endl;
            }
            std::cout << "Rotacionando volumes..." << std::endl;
            rotateByMinimumRectangleResult = RotateVolumeCoordinate::rotateByMinimumRectangle(volumes, minimumRectangle);
            std::cout << "Quantidade de volumes: " << volumes.size() << std::endl;
            std::cout << "minimumZ: " << rotateByMinimumRectangleResult->minimumZ << std::endl;
            std::cout << "maximumZ: " << rotateByMinimumRectangleResult->maximumZ << std::endl;

            std::cout << "Fim processamento dos volumes" << std::endl << std::endl;

            std::cout << "Transformando em regular grid..." << std::endl;
            VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
            RegularGrid<std::shared_ptr<Volume>> regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(
                        volumes,
                        minimumRectangle,
                        rotateByMinimumRectangleResult->minimumZ,
                        rotateByMinimumRectangleResult->maximumZ
                    );
            std::cout << "Fim Transformando em regular grid." << std::endl;

            if (saveGridLithologies)
            {
                auto regularGridShort = std::make_shared<RegularGrid<int>>(
                                           regularGrid.getNumberOfCellsInX(),
                                           regularGrid.getNumberOfCellsInY(),
                                           regularGrid.getNumberOfCellsInZ(),
                                           regularGrid.getCellSizeInX(),
                                           regularGrid.getCellSizeInY(),
                                           regularGrid.getCellSizeInZ(),
                                           regularGrid.getRectanglePoints(),
                                           regularGrid.getZBottom(),
                                           regularGrid.getZTop(),
                                           -1,
                                           0
                                        );
                auto &data = regularGridShort->getData();
                for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
                {
                    for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
                    {
                        for (size_t z = 0; z < regularGrid.getNumberOfCellsInZ(); ++z)
                        {
                            if (regularGrid.getData(x, y, z) != nullptr)
                            {
                                data[x][y][z] = regularGrid.getData(x, y, z)->idLithology;
                            }
                        }
                    }
                }

                std::cout << "Salvando de litologias em arquivo HDF5..." << std::endl;
                RegularGridHdf5Storage<int> regularGridHdf5Writer(gridLithologiesPath, gridLithologiesDatasetName);
                regularGridHdf5Writer.write(*regularGridShort);
                std::cout << "Fim salvando de litologias em arquivo HDF5..." << std::endl;

                return 0;
            }

            std::cout << "Transformando regular grid em impedencia..." << std::endl;
            ImpedanceRegularGridCalculator impedanceCalculator(std::make_shared<Lithology>(0, "undefined", 2.500, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(1, "Mudstone", 2.800, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(2, "Siltstone", 3.000, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(3, "Fine-grained Sandstone", 3.200, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(4, "Medium-grained Sandstone", 3.500, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(5, "Coarse-grained Sandstone", 3.700, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(6, "Very Coarse-grained Sandstone", 4.000, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(7, "Conglomerate", 4.500, 1));
            impedanceCalculator.addLithology(std::make_shared<Lithology>(8, "Volcanic", 6.000, 1));
            impedanceRegularGrid = impedanceCalculator.execute(regularGrid);
            std::cout << "Fim Transformando regular grid em impedencia..." << std::endl;

            if (saveGridImpedance)
            {
                std::cout << "Salvando impedância em arquivo HDF5..." << std::endl;
                RegularGridHdf5Storage<double> regularGridHdf5Writer(gridImpedancePath, gridImpedanceDatasetName);
                regularGridHdf5Writer.write(*impedanceRegularGrid);
                std::cout << "Fim salvando impedância em arquivo HDF5..." << std::endl;
            }
        }
        else
        {
            RegularGridHdf5Storage<double> regularGridHdf5Writer(gridImpedancePath, gridImpedanceDatasetName);
            impedanceRegularGrid = regularGridHdf5Writer.read();
        }

        std::cout << "Transformando impedencia em refletividade..." << std::endl;
        const double undefinedImpedance = 2.500;
        ReflectivityRegularGridCalculator reflectivityCalculator(undefinedImpedance);
        const auto reflectivityRegularGrid = reflectivityCalculator.execute(*impedanceRegularGrid);
        std::cout << "Fim Transformando impedencia em refletividade..." << std::endl;

        if (saveGridReflectivity)
        {
            std::cout << "Salvando refletividade em arquivo HDF5..." << std::endl;
            RegularGridHdf5Storage<double> regularGridHdf5Writer(gridReflectivityPath, gridReflectivityDatasetName);
            regularGridHdf5Writer.write(*reflectivityRegularGrid);
            std::cout << "Fim salvando refletividade em arquivo HDF5..." << std::endl;
        }

        try {
            const int frequenciesSize = static_cast<int>(frequencies.size());
            #pragma omp parallel for
            for (int i = 0; i < frequenciesSize; ++i)
            {
                RickerWaveletCalculator rickerWaveletCalculator;
                rickerWaveletCalculator.setFrequency(frequencies[i]);
                rickerWaveletCalculator.setStep(step);
                const auto wavelet = rickerWaveletCalculator.extract();

                QString gridConvolutionPath  = "D:/gridMolles/convolution/gridMollesConvolution" + QString::number(static_cast<int>(rickerWaveletCalculator.getFrequency())) + "hz_287_735_1252.h5";

                std::cout << std::setprecision(9);
                std::cout << "wavelet size: " << waveletSize << "\n" << std::flush;

                std::cout << "Aplicação da convolução " << frequencies[i] << "hz...\n" << std::flush;
                ConvolutionRegularGridCalculator convolutionCalculator;
                auto seismicRegularGrid = convolutionCalculator.execute(*reflectivityRegularGrid, *wavelet);
                std::cout << "Fim aplicação da convolução " << frequencies[i] << "hz...\n" << std::flush;

                std::cout << "Salvando de convolução em arquivo HDF5 " << frequencies[i] << "hz...\n" << std::flush;
                std::cout << gridConvolutionPath.toStdString() << std::endl;
                RegularGridHdf5Storage<double> regularGridHdf5Writer(gridConvolutionPath, gridConvolutionDatasetName);
                regularGridHdf5Writer.write(*seismicRegularGrid);
                std::cout << "Fim salvando de convolução em arquivo HDF5 " << frequencies[i] << "hz...\n" << std::flush;
            }
        }  catch (std::exception e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }

        return 0;
    }

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

