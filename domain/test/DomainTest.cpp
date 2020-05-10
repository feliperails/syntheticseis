#include <gtest/gtest.h>

#include <domain/src/SeismicWaveVelocityDictionary.h>
#include <domain/src/Lithology.h>

#include <domain/src/Facade.h>
#include <domain/src/LithologyDictionary.h>
#include <domain/src/EclipseGrid.h>

TEST(DomainTest, LithologyDictionaryTest)
{
    using namespace syntheticSeismic::domain;

    LithologyDictionary dictionary;

    dictionary.addLithology(10, QString("e1"));
    dictionary.addLithology(300, QString("e2"));
    dictionary.addLithology(8, QString("e3"));
    dictionary.addLithology(6, QString("e4"));

    EXPECT_EQ(dictionary.lithology(10).id(), 10);
    EXPECT_EQ(dictionary.lithology(300).id(), 300);
    EXPECT_EQ(dictionary.lithology(8).id(), 8);
    EXPECT_EQ(dictionary.lithology(6).id(), 6);
    EXPECT_EQ(dictionary.lithology(100).id(), -1);
    EXPECT_EQ(dictionary.lithology(-1).id(), -1);

    EXPECT_EQ(dictionary.lithology(QString("e1")).name(), QString("e1"));
    EXPECT_EQ(dictionary.lithology(QString("e2")).name(), QString("e2"));
    EXPECT_EQ(dictionary.lithology(QString("e3")).name(), QString("e3"));
    EXPECT_EQ(dictionary.lithology(QString("e4")).name(), QString("e4"));
    EXPECT_EQ(dictionary.lithology(QString("e5")).name(), QString());
    EXPECT_EQ(dictionary.lithology(QString()).name(), QString());

    EXPECT_EQ(dictionary.lithologies().size(), 4);
}

TEST(DomainTest, EclipseGridTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    EclipseGrid eg;

    EXPECT_TRUE(eg.coordinates().isEmpty());
    EXPECT_TRUE(eg.lithologyIds().isEmpty());
    EXPECT_TRUE(eg.zCoordinates().isEmpty());
    EXPECT_EQ(eg.numberOfCellsInX(), 0);
    EXPECT_EQ(eg.numberOfCellsInY(), 0);
    EXPECT_EQ(eg.numberOfCellsInZ(), 0);

    const size_t numberOfCellsInX = 1;
    const size_t numberOfCellsInY = 2;
    const size_t numberOfCellsInZ = 3;
    const QVector<Coordinate> coordinates = {Coordinate(1.0, 1.0, 1.0),
                                             Coordinate(1.0, 1.0, 2.0),
                                             Coordinate(1.0, 3.0, 3.0),
                                             Coordinate(1.0, 3.0, 4.0),
                                             Coordinate(1.0, 1.0, 5.0)};
    const QVector<double> zValues = {5.0, 6.0, 7.0, 8.0, 9.0};
    const QVector<int> lithologyIds = {1, 3, 5, 8, 7};

    eg = EclipseGrid(numberOfCellsInX,
                     numberOfCellsInY,
                     numberOfCellsInZ,
                     coordinates,
                     zValues,
                     lithologyIds);

    EXPECT_EQ(eg.numberOfCellsInX(), numberOfCellsInX);
    EXPECT_EQ(eg.numberOfCellsInY(), numberOfCellsInY);
    EXPECT_EQ(eg.numberOfCellsInZ(), numberOfCellsInZ);
    EXPECT_EQ(eg.coordinates(), coordinates);
    EXPECT_EQ(eg.zCoordinates(), zValues);
    EXPECT_EQ(eg.lithologyIds(), lithologyIds);
}
