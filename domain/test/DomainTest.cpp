#include <gtest/gtest.h>

#include <domain/src/SeismicWaveVelocityDictionary.h>
#include <domain/src/Lithology.h>
#include <domain/src/ExtractVolumes.h>

#include <data/src/Entity.h>
#include <data/mock/DomainObjectMock.h>

#include <data/src/EntityManager.h>
#include <domain/src/LithologyDictionary.h>
#include <domain/src/EclipseGrid.h>
#include <data/src/geometry/Coordinate.h>

TEST(DomainTest, SeismicWaveVelocityDictionaryTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::data;

    Entity e1(QUuid::createUuid(), std::make_shared<Lithology>(), QString("e1"));
    Entity e2(QUuid::createUuid(), std::make_shared<Lithology>(), QString("e2"));
    Entity e3(QUuid::createUuid(), std::make_shared<Lithology>(), QString("e3"));
    Entity e4(QUuid::createUuid(), std::make_shared<FirstObjectMock>(), QString("e4"));

    SeismicWaveVelocityDictionary dictionary;
    ASSERT_EQ(dictionary.lithologyEntities().size(), 0);
    ASSERT_EQ(dictionary.lithologyEntities(), QList<const Entity*>());

    ASSERT_TRUE(dictionary.setVelocity(&e1, 100.5));
    ASSERT_TRUE(dictionary.setVelocity(&e2, 200.0));
    ASSERT_TRUE(dictionary.setVelocity(&e3, 300.0));
    ASSERT_FALSE(dictionary.setVelocity(&e4, 400.0));

    ASSERT_TRUE(qFuzzyCompare(dictionary.velocity(&e1), 100.5));
    ASSERT_TRUE(qFuzzyCompare(dictionary.velocity(&e2), 200.0));
    ASSERT_TRUE(qFuzzyCompare(dictionary.velocity(&e3), 300.0));
    ASSERT_FALSE(qFuzzyCompare(dictionary.velocity(&e4), 400.0));

    ASSERT_FALSE(qFuzzyCompare(dictionary.velocity(&e1), 100.0));

    ASSERT_TRUE(qFuzzyIsNull(dictionary.velocity(&e4)));
    ASSERT_TRUE(qFuzzyIsNull(dictionary.velocity(nullptr)));
    ASSERT_FALSE(qFuzzyCompare(dictionary.velocity(nullptr), 100.0));
}

TEST(DomainTest, LithologyDictionaryTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::data;

    EntityManager entityManager;
    LithologyDictionary dictionary(entityManager);

    entityManager.createEntity(QString("e1"), std::make_shared<Lithology>(10));
    entityManager.createEntity(QString("e2"), std::make_shared<Lithology>(300));
    entityManager.createEntity(QString("e3"), std::make_shared<Lithology>(8));
    entityManager.createEntity(QString("e4"), std::make_shared<Lithology>(6));

    EXPECT_TRUE(dictionary.lithologyEntity(10));
    EXPECT_TRUE(dictionary.lithologyEntity(300));
    EXPECT_TRUE(dictionary.lithologyEntity(8));
    EXPECT_TRUE(dictionary.lithologyEntity(6));
    EXPECT_FALSE(dictionary.lithologyEntity(100));
    EXPECT_FALSE(dictionary.lithologyEntity(-1));

    EXPECT_TRUE(dictionary.lithologyEntity(QString("e1")));
    EXPECT_TRUE(dictionary.lithologyEntity(QString("e2")));
    EXPECT_TRUE(dictionary.lithologyEntity(QString("e3")));
    EXPECT_TRUE(dictionary.lithologyEntity(QString("e4")));
    EXPECT_FALSE(dictionary.lithologyEntity(QString("e5")));
    EXPECT_FALSE(dictionary.lithologyEntity(QString()));
}

TEST(DomainTest, EclipseGridTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::data;
    using namespace syntheticSeismic::geometry;

    EclipseGrid eg;

    EXPECT_TRUE(eg.coordinates().size() == 0);
    EXPECT_TRUE(eg.lithologyIds().size() == 0);
    EXPECT_TRUE(eg.zCoordinates().size() == 0);
    EXPECT_EQ(eg.numberOfCellsInX(), 0);
    EXPECT_EQ(eg.numberOfCellsInY(), 0);
    EXPECT_EQ(eg.numberOfCellsInZ(), 0);

    const size_t numberOfCellsInX = 1;
    const size_t numberOfCellsInY = 2;
    const size_t numberOfCellsInZ = 3;
    const std::vector<Coordinate> coordinates = {Coordinate(1.0, 1.0, 1.0),
                                             Coordinate(1.0, 1.0, 2.0),
                                             Coordinate(1.0, 3.0, 3.0),
                                             Coordinate(1.0, 3.0, 4.0),
                                             Coordinate(1.0, 1.0, 5.0)};
    const std::vector<double> zValues = {5.0, 6.0, 7.0, 8.0, 9.0};
    const std::vector<int> lithologyIds = {1, 3, 5, 8, 7};

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

TEST(DomainTest, ExtractVolumesTest)
{
    using namespace std;
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::data;
    using namespace syntheticSeismic::geometry;

    const size_t numberOfCellsInX = 2;
    const size_t numberOfCellsInY = 3;
    const size_t numberOfCellsInZ = 4;

    vector<Coordinate> coordinates = {
        Coordinate(1000, 2000, 1000),
        Coordinate(1100, 2000, 1100),
        Coordinate(1050, 2000, 1000),
        Coordinate(1150, 2000, 1100),
        Coordinate(1100, 2000, 1000),
        Coordinate(1200, 2000, 1100),
        Coordinate(1000, 2200, 1000),
        Coordinate(1100, 2200, 1100),
        Coordinate(1100, 2200, 1000),
        Coordinate(1200, 2200, 1100),
        Coordinate(1200, 2200, 1000),
        Coordinate(1300, 2200, 1100),
        Coordinate(1000, 2600, 1000),
        Coordinate(1100, 2600, 1100),
        Coordinate(1150, 2600, 1000),
        Coordinate(1250, 2600, 1100),
        Coordinate(1300, 2600, 1000),
        Coordinate(1400, 2600, 1100),
        Coordinate(1000, 3200, 1000),
        Coordinate(1100, 3200, 1100),
        Coordinate(1200, 3200, 1000),
        Coordinate(1300, 3200, 1100),
        Coordinate(1400, 3200, 1000),
        Coordinate(1500, 3200, 1100)
    };

    vector<double> zCoordinates = {
        1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
        1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
        1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
        1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
        1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
        1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300,
        1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300,
        1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400
    };

    vector<int> lithologyIds = {
         1,  2,  3,  4,  5,  6,
         7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24
    };

    EclipseGrid eclipseGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ, coordinates, zCoordinates, lithologyIds);


}
