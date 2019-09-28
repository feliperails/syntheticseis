#include <gtest/gtest.h>

#include <geometry/src/Coordinate.h>
#include <domain/src/EclipseGrid.h>
#include <dataIO/src/reader/EclipseGridReader.h>

#include <QTime>

TEST(DataIOTest, EclipseGridReaderTest)
{
    using namespace  invertseis::dataIO;
    using namespace  invertseis::domain;
    using namespace  invertseis::geometry;

    invertseis::dataIO::EclipseGridReader reader(QLatin1String("EclipseGridTest.grdecl"));
    bool ok = false;
    const EclipseGrid eg = reader.read(&ok);

    ASSERT_TRUE(ok);

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

    EXPECT_EQ(eg.zValues(), zValues);

    const QVector<int> lithologyIds = {1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6,1,2,3,4,5,6};
    EXPECT_EQ(eg.lithologyIds(), lithologyIds);
}

TEST(DataIOTest, EclipseGridReaderPerformanceTest)
{
    using namespace  invertseis::dataIO;
    using namespace  invertseis::domain;

    invertseis::dataIO::EclipseGridReader reader(QLatin1String("EclipseGridPerformanceTest.grdecl"));
    bool ok = false;
    QTime time;
    time.start();
    reader.read(&ok);

    std::cout << "Tempo gasto: " << time.elapsed() << std::endl;

    ASSERT_TRUE(ok);
}
