#include <gtest/gtest.h>
#include <domain/src/SeismicWaveVelocityDictionary.h>
#include <domain/src/Lithology.h>
#include <domain/src/ExtractMinimumRectangle2D.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/Facade.h>
#include <domain/src/LithologyDictionary.h>
#include <domain/src/EclipseGrid.h>
#include <domain/src/RotateVolumeCoordinate.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <domain/mock/DomainMock.h>
#include <storage/src/reader/EclipseGridReader.h>
#include <QFile>
#include <QTextStream>
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

    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(0), 0);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(2), 1);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(4), 2);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(6), 3);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(1), 4);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(3), 5);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(5), 6);
    EXPECT_EQ(ExtractVolumes::calculateReorderedIndex(7), 7);
}

TEST(DomainTest, ExtractVolumesOfFirstLayerTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    const EclipseGrid eclipseGrid = DomainTestValues::eclipseGridFromSimpleGrid();

    const auto volumesOfFirstLayerCompare = DomainTestValues::volumesOfFirstLayerFromSimpleGrid();

    const auto volumes = ExtractVolumes::extractFirstLayerFrom(eclipseGrid);
    for (size_t volumeIndex = 0; volumeIndex < volumes.size(); ++volumeIndex)
    {
        for (size_t pointIndex = 0; pointIndex < volumes[volumeIndex]->points.size(); ++pointIndex)
        {
            EXPECT_DOUBLE_EQ(volumes[volumeIndex]->points[pointIndex].x, volumesOfFirstLayerCompare[volumeIndex]->points[pointIndex].x)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate X";
            EXPECT_DOUBLE_EQ(volumes[volumeIndex]->points[pointIndex].y, volumesOfFirstLayerCompare[volumeIndex]->points[pointIndex].y)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Y";
            EXPECT_DOUBLE_EQ(volumes[volumeIndex]->points[pointIndex].z, volumesOfFirstLayerCompare[volumeIndex]->points[pointIndex].z)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Z";
        }
    }
}

TEST(DomainTest, ExtractVolumesTest)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    EclipseGrid eclipseGrid = DomainTestValues::eclipseGridFromSimpleGrid();

    const auto volumesCompare = DomainTestValues::volumesFromSimpleGrid();

    const auto volumesOfFirstLayer = DomainTestValues::volumesOfFirstLayerFromSimpleGrid();
    const auto volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, eclipseGrid);
    for (size_t volumeIndex = 0; volumeIndex < volumes.size(); ++volumeIndex)
    {
        for (size_t pointIndex = 0; pointIndex < volumes[volumeIndex]->points.size(); ++pointIndex)
        {
            EXPECT_DOUBLE_EQ(volumes[volumeIndex]->points[pointIndex].x, volumesCompare[volumeIndex]->points[pointIndex].x)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate X";
            EXPECT_DOUBLE_EQ(volumes[volumeIndex]->points[pointIndex].y, volumesCompare[volumeIndex]->points[pointIndex].y)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Y";
            EXPECT_DOUBLE_EQ(volumes[volumeIndex]->points[pointIndex].z, volumesCompare[volumeIndex]->points[pointIndex].z)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Z";
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
        EXPECT_DOUBLE_EQ(minimumRectangle[i].x, minimumRectangleCompare[i].x);
        EXPECT_DOUBLE_EQ(minimumRectangle[i].y, minimumRectangleCompare[i].y);
    }
}

TEST(DomainTest, RotateVolumeCoordinateWithSimpleGridTest)
{
    using namespace syntheticSeismic::domain;

    auto volumes = DomainTestValues::volumesFromSimpleGridRotated30Degrees();
    const auto volumesCompare = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    const auto minimumRectangle = extractMinimumRectangle2D::extractFrom(volumes);
    const auto referencePointAndAngleInRadians = RotateVolumeCoordinate::calculateReferencePoint(minimumRectangle);
    const auto referencePointAndAngleInRadiansCompare = DomainTestValues::referencePointAndAngleInRadiansFromSimpleGridRotated30Degrees();

    EXPECT_DOUBLE_EQ(referencePointAndAngleInRadians.first.x, referencePointAndAngleInRadiansCompare.first.x);
    EXPECT_DOUBLE_EQ(referencePointAndAngleInRadians.first.y, referencePointAndAngleInRadiansCompare.first.y);
    EXPECT_DOUBLE_EQ(referencePointAndAngleInRadians.second, referencePointAndAngleInRadiansCompare.second);

    RotateVolumeCoordinate::rotateByMinimumRectangle(volumes, minimumRectangle);

    for (size_t volumeIndex = 0; volumeIndex < volumes.size(); ++volumeIndex)
    {
        for (size_t pointIndex = 0; pointIndex < volumes[volumeIndex]->points.size(); ++pointIndex)
        {
            // Uses 5 decimal places for comparison, as the values in DomainTestValues have been stored rounded up to 5 decimal places.
            EXPECT_DOUBLE_EQ(ceil(volumes[volumeIndex]->points[pointIndex].x * 100000) / 100000, volumesCompare[volumeIndex]->points[pointIndex].x)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate X";
            EXPECT_DOUBLE_EQ(ceil(volumes[volumeIndex]->points[pointIndex].y * 100000) / 100000, volumesCompare[volumeIndex]->points[pointIndex].y)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Y";
            EXPECT_DOUBLE_EQ(ceil(volumes[volumeIndex]->points[pointIndex].z * 100000) / 100000, volumesCompare[volumeIndex]->points[pointIndex].z)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Z";
        }
    }
}

TEST(DomainTest, VolumeToRegularGrid)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    const auto volumes = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    auto regularGridCompare = DomainTestValues::regularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumes);

    EXPECT_EQ(regularGrid.getNumberOfCellsInX(), regularGridCompare.getNumberOfCellsInX());
    EXPECT_EQ(regularGrid.getNumberOfCellsInY(), regularGridCompare.getNumberOfCellsInY());
    EXPECT_EQ(regularGrid.getNumberOfCellsInZ(), regularGridCompare.getNumberOfCellsInZ());
    EXPECT_DOUBLE_EQ(std::round(regularGrid.getCellSizeInX()), regularGridCompare.getCellSizeInX());
    EXPECT_DOUBLE_EQ(std::round(regularGrid.getCellSizeInY()), regularGridCompare.getCellSizeInY());
    EXPECT_DOUBLE_EQ(std::round(regularGrid.getCellSizeInZ()), regularGridCompare.getCellSizeInZ());

    for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
        {
            for (size_t z = 0; z < regularGrid.getNumberOfCellsInZ(); ++z)
            {
                if (regularGrid.getData()[x][y][z] != nullptr)
                {
                    EXPECT_EQ(regularGrid.getData()[x][y][z]->indexVolume, regularGridCompare.getData()[x][y][z])
                            << "Cell error: " << x << ", " << y << ", " << z;
                }
                else
                {
                    EXPECT_EQ(Volume::UNDEFINED_LITHOLOGY, regularGridCompare.getData()[x][y][z])
                            << "Cell error: " << x << ", " << y << ", " << z;
                }
            }
        }
    }
}
