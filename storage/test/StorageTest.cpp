#include <gtest/gtest.h>

#include "geometry/src/Coordinate.h"
#include "domain/src/EclipseGrid.h"
#include "storage/src/reader/EclipseGridReader.h"
#include "storage/src/reader/GrdSurfaceReader.h"
#include "storage/src/writer/EclipseGridWriter.h"
#include "storage/src/RegularGridHdf5Storage.h"
#include "storage/test/StorageTestValues.h"

#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QTime>

namespace {
const QLatin1String ECLIPSEGRID_SMALL_FILENAME = QLatin1String("EclipseGridTest.grdecl");
const QLatin1String ECLIPSEGRID_SMALL_OUTPUT_FILENAME = QLatin1String("EclipseGridTestOutput.grdecl");
const QLatin1String ECLIPSEGRID_BIG_FILENAME = QLatin1String("EclipseGridPerformanceTest.grdecl");
const QLatin1String ECLIPSEGRID_BIG_OUTPUT_FILENAME = QLatin1String("EclipseGridPerformanceTestOutput.grdecl");
}

TEST(storageTest, EclipseGridReaderTest)
{
    using namespace  syntheticSeismic::storage;
    using namespace  syntheticSeismic::domain;
    using namespace  syntheticSeismic::geometry;

    syntheticSeismic::storage::EclipseGridReader reader(ECLIPSEGRID_SMALL_FILENAME);
    QString error;
    const EclipseGrid eg = reader.read(error);

    ASSERT_TRUE(error.isEmpty());

    EXPECT_EQ(eg.numberOfCellsInX(), 2);
    EXPECT_EQ(eg.numberOfCellsInY(), 3);
    EXPECT_EQ(eg.numberOfCellsInZ(), 4);

    const std::vector<Coordinate> coordinates = {Coordinate(1000.0, 2000.0, 1000.0),
                                              Coordinate(1100.0, 2000.0, 1100.0),
                                              Coordinate(1040.0, 2000.0, 1000.0),
                                              Coordinate(1150.0, 2000.0, 1100.0),
                                              Coordinate(1120.0, 2000.0, 1000.0),
                                              Coordinate(1200.0, 2000.0, 1100.0),
                                              Coordinate(1000.0, 2200.0, 1000.0),
                                              Coordinate(1100.0, 2200.0, 1100.0),
                                              Coordinate(1100.0, 2200.0, 1000.0),
                                              Coordinate(1200.0, 2200.0, 1100.0),
                                              Coordinate(1200.0, 2200.0, 1000.0),
                                              Coordinate(1300.0, 2200.0, 1100.0),
                                              Coordinate(1000.0, 2600.0, 1000.0),
                                              Coordinate(1100.0, 2600.0, 1100.0),
                                              Coordinate(1150.0, 2600.0, 1000.0),
                                              Coordinate(1250.0, 2600.0, 1100.0),
                                              Coordinate(1300.0, 2600.0, 1000.0),
                                              Coordinate(1400.0, 2600.0, 1100.0),
                                              Coordinate(1000.0, 3200.0, 1000.0),
                                              Coordinate(1100.0, 3200.0, 1100.0),
                                              Coordinate(1200.0, 3200.0, 1000.0),
                                              Coordinate(1300.0, 3200.0, 1100.0),
                                              Coordinate(1400.0, 3200.0, 1000.0),
                                              Coordinate(1500.0, 3200.0, 1100.0)};

    EXPECT_EQ(eg.coordinates(), coordinates);

    std::vector<double> zValues;
    zValues.reserve((24 * 2) * 4); // 4 camadas

    // Primeira camada
    zValues.insert(zValues.end(), 24, 1000.0);
    zValues.insert(zValues.end(), 24, 1100.0);

    // Segunda camada
    zValues.insert(zValues.end(), 24, 1100.0);
    zValues.insert(zValues.end(), 24, 1200.0);

    // Tereceira camada
    zValues.insert(zValues.end(), 24, 1200.0);
    zValues.insert(zValues.end(), 24, 1300.0);

    // Quarta camada
    zValues.insert(zValues.end(), 24, 1300.0);
    zValues.insert(zValues.end(), 24, 1400.0);

    EXPECT_EQ(eg.zCoordinates(), zValues);

    const std::vector<int> lithologyIds = {1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6};
    EXPECT_EQ(eg.lithologyIds(), lithologyIds);
}

TEST(storageTest, EclipseGridReaderPerformanceTest)
{
    using namespace  syntheticSeismic::storage;
    using namespace  syntheticSeismic::domain;

    syntheticSeismic::storage::EclipseGridReader reader(ECLIPSEGRID_BIG_FILENAME);
    QString error;
    QTime time;
    time.start();
    reader.read(error);

    std::cout << "Tempo gasto: " << time.elapsed() << std::endl;

    ASSERT_TRUE(error.isEmpty());
}

namespace {

QByteArray calculateSH1Hash(const QString& fileName)
{
    QFile file(fileName);
    EXPECT_TRUE(file.open(QIODevice::ReadOnly));

    QByteArray byteArray = file.readAll();
    EXPECT_FALSE(byteArray.isEmpty());

    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(byteArray);

    return hash.result();
}

bool compareFiles(const QString& firstFilename, const QString& secondFilename)
{
    QFile file01(firstFilename);
    QFile file02(secondFilename);

    if(!(file01.open(QFile::ReadOnly) && file02.open(QFile::ReadOnly))){
        return false;
    }

    QTextStream stream01(&file01);
    QTextStream stream02(&file02);

    while(!(stream01.atEnd() && stream02.atEnd())){

        const QString line01 = stream01.readLine();
        const QString line02 = stream02.readLine();

        if (line01 != line02) {
            return false;
        }
    }

    return stream01.atEnd() && stream02.atEnd();
}

}

TEST(storageTest, EclipseGridWriterTest)
{
    using namespace syntheticSeismic::storage;
    using namespace syntheticSeismic::domain;

    syntheticSeismic::storage::EclipseGridReader reader(ECLIPSEGRID_SMALL_FILENAME);
    QString error;
    syntheticSeismic::domain::EclipseGrid eclipseGrid = reader.read(error);
    ASSERT_TRUE(error.isEmpty());

    syntheticSeismic::storage::EclipseGridWriter writer(ECLIPSEGRID_SMALL_OUTPUT_FILENAME);
    EXPECT_TRUE(writer.write(eclipseGrid));

    EXPECT_TRUE(compareFiles(ECLIPSEGRID_SMALL_FILENAME, ECLIPSEGRID_SMALL_OUTPUT_FILENAME));
}

TEST(storageTest, RegularGridHdf5StorageTest)
{
    using namespace syntheticSeismic::storage;
    using namespace syntheticSeismic::domain;

    double epsilon = std::pow(10, 10);

    auto regularGridCompare = StorageTestValues::regularGridImpedanceFiveFiveFive();

    RegularGridHdf5Storage<double> regularGridHdf5Writer("gridMollesImpedance_5_5_5.h5", "/data");
    auto regularGrid = regularGridHdf5Writer.read();

    EXPECT_EQ(regularGrid->getNumberOfCellsInX(), regularGridCompare.getNumberOfCellsInX());
    EXPECT_EQ(regularGrid->getNumberOfCellsInY(), regularGridCompare.getNumberOfCellsInY());
    EXPECT_EQ(regularGrid->getNumberOfCellsInZ(), regularGridCompare.getNumberOfCellsInZ());
    EXPECT_LT(std::abs(regularGrid->getCellSizeInX() - regularGridCompare.getCellSizeInX()), epsilon);
    EXPECT_LT(std::abs(regularGrid->getCellSizeInY() - regularGridCompare.getCellSizeInY()), epsilon);
    EXPECT_LT(std::abs(regularGrid->getCellSizeInZ() - regularGridCompare.getCellSizeInZ()), epsilon);
    EXPECT_EQ(regularGrid->getNoDataValue(), regularGridCompare.getNoDataValue());

    auto limitX = std::min(regularGrid->getNumberOfCellsInX(), regularGridCompare.getNumberOfCellsInX());
    auto limitY = std::min(regularGrid->getNumberOfCellsInY(), regularGridCompare.getNumberOfCellsInY());
    auto limitZ = std::min(regularGrid->getNumberOfCellsInZ(), regularGridCompare.getNumberOfCellsInZ());

    for (size_t x = 0; x < limitX; ++x)
    {
        for (size_t y = 0; y < limitY; ++y)
        {
            for (size_t z = 0; z < limitZ; ++z)
            {
                EXPECT_LT(std::abs(regularGrid->getData(x, y, z) - regularGridCompare.getData(x, y, z)), epsilon);
            }
        }
    }
}

TEST(storageTest, GrdSurfaceReader)
{
    using namespace syntheticSeismic::storage;
    using namespace syntheticSeismic::domain;

    double epsilon = std::pow(10, 10);

    GrdSurface<double> surfaceCompare = StorageTestValues::simpleGrdSurface();

    GrdSurfaceReader<double> reader("grdDsaaTest.grd");
    GrdSurface<double> surface = reader.read();

    EXPECT_LT(std::abs(surface.getXMin() - surfaceCompare.getXMin()), epsilon);
    EXPECT_LT(std::abs(surface.getYMin() - surfaceCompare.getYMin()), epsilon);
    EXPECT_LT(std::abs(surface.getXMax() - surfaceCompare.getXMax()), epsilon);
    EXPECT_LT(std::abs(surface.getYMax() - surfaceCompare.getYMax()), epsilon);

    for (size_t i = 0; i < surface.getData().size(); ++i)
    {
        for (size_t j = 0; j < surface.getData()[0].size(); ++j)
        {
            EXPECT_LT(std::abs(surface.getData()[i][j] - surfaceCompare.getData()[i][j]), epsilon);
        }
    }
}
