#include <gtest/gtest.h>

#include "geometry/src/Coordinate.h"
#include "domain/src/EclipseGrid.h"
#include "storage/src/reader/EclipseGridReader.h"
#include "storage/src/writer/EclipseGridWriter.h"

#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QTime>

namespace{
const QLatin1String ECLIPSEGRID_SMALL_FILENAME = QLatin1String("EclipseGridTest.grdecl");
const QLatin1String ECLIPSEGRID_SMALL_OUTPUT_FILENAME = QLatin1String("EclipseGridTestOutput.grdecl");
const QLatin1String ECLIPSEGRID_BIG_FILENAME = QLatin1String("EclipseGridPerformanceTest.grdecl");
const QLatin1String ECLIPSEGRID_BIG_OUTPUT_FILENAME = QLatin1String("EclipseGridPerformanceTestOutput.grdecl");

}

TEST(DataIOTest, EclipseGridReaderTest)
{
    using namespace  syntheticSeismic::dataIO;
    using namespace  syntheticSeismic::domain;
    using namespace  syntheticSeismic::geometry;

    syntheticSeismic::dataIO::EclipseGridReader reader(ECLIPSEGRID_SMALL_FILENAME);
    QString error;
    const EclipseGrid eg = reader.read(error);

    ASSERT_TRUE(error.isEmpty());

    EXPECT_EQ(eg.numberOfCellsInX(), 2);
    EXPECT_EQ(eg.numberOfCellsInY(), 3);
    EXPECT_EQ(eg.numberOfCellsInZ(), 4);

    const QVector<Coordinate> coordinates = {Coordinate(1000.0, 2000.0, 1000.0),
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

    QVector<double> zValues;
    zValues.reserve((24 * 2) * 4); // 4 camadas

    // Primeira camada
    zValues.append(QVector<double>(24, 1000.0));
    zValues.append(QVector<double>(24, 1100.0));

    // Segunda camada
    zValues.append(QVector<double>(24, 1100.0));
    zValues.append(QVector<double>(24, 1200.0));

    // Tereceira camada
    zValues.append(QVector<double>(24, 1200.0));
    zValues.append(QVector<double>(24, 1300.0));

    // Quarta camada
    zValues.append(QVector<double>(24, 1300.0));
    zValues.append(QVector<double>(24, 1400.0));

    EXPECT_EQ(eg.zCoordinates(), zValues);

    const QVector<int> lithologyIds = {1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6};
    EXPECT_EQ(eg.lithologyIds(), lithologyIds);
}

TEST(DataIOTest, EclipseGridReaderPerformanceTest)
{
    using namespace  syntheticSeismic::dataIO;
    using namespace  syntheticSeismic::domain;

    syntheticSeismic::dataIO::EclipseGridReader reader(ECLIPSEGRID_BIG_FILENAME);
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

TEST(DataIOTest, EclipseGridWriterTest)
{
    using namespace  syntheticSeismic::dataIO;
    using namespace  syntheticSeismic::domain;

    syntheticSeismic::dataIO::EclipseGridReader reader(ECLIPSEGRID_SMALL_FILENAME);
    QString error;
    syntheticSeismic::domain::EclipseGrid eclipseGrid = reader.read(error);
    ASSERT_TRUE(error.isEmpty());

    syntheticSeismic::dataIO::EclipseGridWriter writer(ECLIPSEGRID_SMALL_OUTPUT_FILENAME);
    EXPECT_TRUE(writer.write(eclipseGrid));

    EXPECT_TRUE(compareFiles(ECLIPSEGRID_SMALL_FILENAME, ECLIPSEGRID_SMALL_OUTPUT_FILENAME));
}

