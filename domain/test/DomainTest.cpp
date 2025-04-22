#include <gtest/gtest.h>
#include <domain/src/EclipseGrid.h>
#include <domain/src/EquationPlane.h>
#include <domain/src/ExtractMinimumRectangle2D.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <domain/src/RotateVolumeCoordinate.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <storage/src/reader/GrdSurfaceReader.h>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "DomainTestValues.h"

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
    const std::vector<Coordinate> coordinates = {
        Coordinate(1.0, 1.0, 1.0),
        Coordinate(1.0, 1.0, 2.0),
        Coordinate(1.0, 3.0, 3.0),
        Coordinate(1.0, 3.0, 4.0),
        Coordinate(1.0, 1.0, 5.0)
    };
    const std::vector<double> zValues = {5.0, 6.0, 7.0, 8.0, 9.0};
    const std::vector<int> lithologyIds = {1, 3, 5, 8, 7};
    const std::vector<int> faciesAssociationIds = {1, 2, 3, 4, 5};
    const std::vector<double> ages = {0.0, 0.0, 0.0, 0.0, 0.0};
    const std::vector<bool> actnums = {true, true, true, true, true};

    EclipseGrid eg2(numberOfCellsInX,
                    numberOfCellsInY,
                    numberOfCellsInZ,
                    coordinates,
                    zValues,
                    lithologyIds,
                    faciesAssociationIds,
                    ages,
                    actnums);

    EXPECT_EQ(eg2.numberOfCellsInX(), numberOfCellsInX);
    EXPECT_EQ(eg2.numberOfCellsInY(), numberOfCellsInY);
    EXPECT_EQ(eg2.numberOfCellsInZ(), numberOfCellsInZ);
    EXPECT_EQ(eg2.coordinates(), coordinates);
    EXPECT_EQ(eg2.zCoordinates(), zValues);
    EXPECT_EQ(eg2.lithologyIds(), lithologyIds);
    EXPECT_EQ(eg2.faciesAssociationIds(), faciesAssociationIds);
    EXPECT_EQ(eg2.actnums(), actnums);
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

    const auto eclipseGrid = DomainTestValues::eclipseGridFromSimpleGrid();

    const auto volumesOfFirstLayerCompare = DomainTestValues::volumesOfFirstLayerFromSimpleGrid();

    const auto volumes = ExtractVolumes::extractFirstLayerFrom(*eclipseGrid);
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

    const auto eclipseGrid = DomainTestValues::eclipseGridFromSimpleGrid();

    const auto volumesCompare = DomainTestValues::volumesFromSimpleGrid();

    const auto volumesOfFirstLayer = DomainTestValues::volumesOfFirstLayerFromSimpleGrid();
    const auto volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesOfFirstLayer, *eclipseGrid);
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

TEST(DomainTest, RotateVolumeCoordinateWithSimpleGridTest)
{
    using namespace syntheticSeismic::domain;

    auto volumes = DomainTestValues::volumesFromSimpleGridRotated30Degrees();
    const auto volumesCompareResult = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    const auto minimumRectangle = ExtractMinimumRectangle2D::extractFrom(volumes);
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
            EXPECT_DOUBLE_EQ(ceil(volumes[volumeIndex]->points[pointIndex].x * 100000) / 100000, volumesCompareResult.volumes[volumeIndex]->points[pointIndex].x)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate X";
            EXPECT_DOUBLE_EQ(ceil(volumes[volumeIndex]->points[pointIndex].y * 100000) / 100000, volumesCompareResult.volumes[volumeIndex]->points[pointIndex].y)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Y";
            EXPECT_DOUBLE_EQ(ceil(volumes[volumeIndex]->points[pointIndex].z * 100000) / 100000, volumesCompareResult.volumes[volumeIndex]->points[pointIndex].z)
                    << "Volume error: " << volumeIndex << " point error: " << pointIndex << " coordinate Z";
        }
    }
}

TEST(DomainTest, VolumeToRegularGrid)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    const auto volumesResult = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    auto regularGridCompare = DomainTestValues::regularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumesResult.volumes, volumesResult.rectanglePoints, volumesResult.zBottom, volumesResult.zTop);

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

TEST(DomainTest, EquationPlane)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    EquationPlane equationPlane(Point3D(-2.0, 1.0, 0.0), Point3D(-1.0, 4.0, 2.0), Point3D(0.0, -2.0, 2.0));

    const double epsilon = std::pow(10, -10);

    EXPECT_LT(std::abs(equationPlane.getNormalVector().x - -12), epsilon);
    EXPECT_LT(std::abs(equationPlane.getNormalVector().y - -2), epsilon);
    EXPECT_LT(std::abs(equationPlane.getNormalVector().z - 9), epsilon);
}

