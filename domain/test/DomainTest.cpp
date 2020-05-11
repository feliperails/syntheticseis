#include <gtest/gtest.h>
#include <domain/src/SeismicWaveVelocityDictionary.h>
#include <domain/src/Lithology.h>
#include <domain/src/ExtractMinimumRectangle2D.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/Facade.h>
#include <domain/src/LithologyDictionary.h>
#include <domain/src/EclipseGrid.h>
#include <domain/mock/DomainMock.h>
#include "DomainTestValues.h"

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

    EXPECT_EQ(eg.coordinates().size(), 0);
    EXPECT_EQ(eg.lithologyIds().size(), 0);
    EXPECT_EQ(eg.zCoordinates().size(), 0);
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

TEST(DomainTest, ExtractVolumesCalculateReorderedIndex)
{
    using namespace syntheticSeismic::domain;

    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(0), (0));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(2), (1));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(4), (2));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(6), (3));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(1), (4));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(3), (5));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(5), (6));
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(7), (7));
}

TEST(DomainTest, ExtractVolumesOfFirstLayerTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    EclipseGrid eclipseGrid = DomainTestValues::eclipseGridFromSimpleGrid();

    auto volumesOfFirstLayerCompare = DomainTestValues::volumesOfFirstLayerFromSimpleGrid();

    auto volumes = ExtractVolumes::extractFirstLayerFrom(eclipseGrid);
    for (size_t volumeIndex = 0; volumeIndex < volumes.size(); ++volumeIndex)
    {
        for (size_t pointIndex = 0; pointIndex < volumes[volumeIndex].m_points.size(); ++pointIndex)
        {
            EXPECT_EQ(volumes[volumeIndex].m_points[pointIndex].x, volumesOfFirstLayerCompare[volumeIndex].m_points[pointIndex].x)
                    << "Volume error: " << volumeIndex << " - point error: " << pointIndex;
        }
    }
}

TEST(DomainTest, ExtractVolumesTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    EclipseGrid eclipseGrid = DomainTestValues::eclipseGridFromSimpleGrid();

    auto volumesCompare = DomainTestValues::volumesFromSimpleGrid();

    auto volumesOfFirstLayer = DomainTestValues::volumesOfFirstLayerFromSimpleGrid();
    auto volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, eclipseGrid);
    for (size_t volumeIndex = 0; volumeIndex < volumes.size(); ++volumeIndex)
    {
        for (size_t pointIndex = 0; pointIndex < volumes[volumeIndex].m_points.size(); ++pointIndex)
        {
            EXPECT_EQ(volumes[volumeIndex].m_points[pointIndex].x, volumesCompare[volumeIndex].m_points[pointIndex].x)
                    << "Volume error: " << volumeIndex << " - point error: " << pointIndex;
        }
    }
}

TEST(DomainTest, ExtractMinimumRectangle2DTest)
{
    using namespace syntheticSeismic::domain;

    const auto volumes = DomainTestValues::volumesFromSimpleGrid();

    const auto minimumRectangle = extractMinimumRectangle2D::extractFrom(volumes);

    const auto minimumRectangleCompare = DomainTestValues::minimumRectangleFromSimpleGrid();
    for (size_t i = 0; i < minimumRectangle.size(); ++i)
    {
        EXPECT_EQ(minimumRectangle[i].x, minimumRectangleCompare[i].x);
        EXPECT_EQ(minimumRectangle[i].y, minimumRectangleCompare[i].y);
    }
}
