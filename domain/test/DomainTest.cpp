#include <gtest/gtest.h>
#include <domain/src/ConvolutionRegularGridCalculator.h>
#include <domain/src/EclipseGrid.h>
#include <domain/src/ExtractMinimumRectangle2D.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/Facade.h>
#include <domain/src/Lithology.h>
#include <domain/src/LithologyDictionary.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <domain/src/ReflectivityRegularGridCalculator.h>
#include <domain/src/RickerWaveletCalculator.h>
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

    dictionary.addLithology(10, QString("e1"), 0.0, 0.0);
    dictionary.addLithology(300, QString("e2"), 0.0, 0.0);
    dictionary.addLithology(8, QString("e3"), 0.0, 0.0);
    dictionary.addLithology(6, QString("e4"), 0.0, 0.0);

    EXPECT_EQ(dictionary.lithology(10).getId(), 10);
    EXPECT_EQ(dictionary.lithology(300).getId(), 300);
    EXPECT_EQ(dictionary.lithology(8).getId(), 8);
    EXPECT_EQ(dictionary.lithology(6).getId(), 6);
    EXPECT_EQ(dictionary.lithology(100).getId(), -1);
    EXPECT_EQ(dictionary.lithology(-1).getId(), -1);

    EXPECT_EQ(dictionary.lithology(QString("e1")).getName(), QString("e1"));
    EXPECT_EQ(dictionary.lithology(QString("e2")).getName(), QString("e2"));
    EXPECT_EQ(dictionary.lithology(QString("e3")).getName(), QString("e3"));
    EXPECT_EQ(dictionary.lithology(QString("e4")).getName(), QString("e4"));
    EXPECT_EQ(dictionary.lithology(QString("e5")).getName(), QString());
    EXPECT_EQ(dictionary.lithology(QString()).getName(), QString());

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
    const auto minimumRectangle = ExtractMinimumRectangle2D::extractFrom(volumes);

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

TEST(DomainTest, ImpedanceCalculator)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    const auto volumes = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    const auto impedanceRegularGridCompare = DomainTestValues::impedanceRegularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;
    double epsilon = std::pow(10, -10);

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumes);

    ImpedanceRegularGridCalculator impedanceCalculator(std::make_shared<Lithology>(0, "undefined", 2.500, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(1, "argilito - mudstone", 2.800, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(3, "siltito - siltite", 3.000, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(5, "arn mf - fine-grained sandstone", 3.200, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(7, "arn f - fine-grained sandstone", 3.200, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(9, "arn m - medium-grained sandstone", 3.500, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(11, "arn g - coarse-grained sandstone", 3.700, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(13, "arn mg - very coarse-grained sandstone", 4.000, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(15, "cgl gr - conglomerate", 4.500, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(17, "cgl sx - conglomerate", 4.500, 1));
    impedanceCalculator.addLithology(std::make_shared<Lithology>(24, "vulcanica - volcanic", 6.000, 1));
    std::shared_ptr<RegularGrid<double>> impedanceRegularGrid = impedanceCalculator.execute(regularGrid);

    for (size_t x = 0; x < impedanceRegularGrid->getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < impedanceRegularGrid->getNumberOfCellsInY(); ++y)
        {
            for (size_t z = 0; z < impedanceRegularGrid->getNumberOfCellsInZ(); ++z)
            {
                EXPECT_LT(std::abs(impedanceRegularGrid->getData(x, y, z) - impedanceRegularGridCompare.getData(x, y, z)), epsilon);
            }
        }
    }
}

TEST(DomainTest, ReflectivityCalculator)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    auto impedanceRegularGrid = DomainTestValues::impedanceRegularGridFromSimpleGridRotated30Degrees();
    const auto reflectivityRegularGridCompare = DomainTestValues::reflectivityRegularGridFromSimpleGridRotated30Degrees();
    double epsilon = std::pow(10, -4);
    const double undefinedImpedance = 2.5;

    ReflectivityRegularGridCalculator reflectivityCalculator(undefinedImpedance);
    const auto reflectivityRegularGrid = reflectivityCalculator.execute(impedanceRegularGrid);

    for (size_t x = 0; x < impedanceRegularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < impedanceRegularGrid.getNumberOfCellsInY(); ++y)
        {
            for (size_t z = 0; z < impedanceRegularGrid.getNumberOfCellsInZ(); ++z)
            {
                EXPECT_LT(std::abs(reflectivityRegularGrid->getData(x, y, z) - reflectivityRegularGridCompare.getData(x, y, z)), epsilon);
            }
        }
    }
}

TEST(DomainTest, RickerWavelet)
{
    using namespace syntheticSeismic::domain;

    auto waveletCompare = DomainTestValues::rickerWavelet();

    RickerWaveletCalculator rickerCalculator;
    rickerCalculator.setStep(waveletCompare.getStep());
    rickerCalculator.setFrequency(waveletCompare.getFrequency());
    auto wavelet = rickerCalculator.extract();

    EXPECT_EQ(wavelet->getTraces().size(), waveletCompare.getTraces().size());

    if (wavelet->getTraces().size() == waveletCompare.getTraces().size())
    {
        double epsilon = std::pow(10, -14);
        for (size_t i = 0; i < wavelet->getTraces().size(); ++i)
        {
            EXPECT_LT(std::abs(wavelet->getTraces()[i] - waveletCompare.getTraces()[i]), epsilon);
        }
    }
}

TEST(DomainTest, Convolution)
{
    using namespace syntheticSeismic::domain;

    auto regularGrid = DomainTestValues::regularGridToTestConvolution();
    auto wavelet = DomainTestValues::waveletToTestConvolution();
    auto convolutionRegularGridCompare = DomainTestValues::regularGridConvolution();
    double epsilon = std::pow(10, -10);

    ConvolutionRegularGridCalculator convolutionCalculator;
    auto convolutionRegularGrid = convolutionCalculator.execute(regularGrid, wavelet);

    EXPECT_EQ(convolutionRegularGrid->getNumberOfCellsInX(), convolutionRegularGridCompare.getNumberOfCellsInX());
    EXPECT_EQ(convolutionRegularGrid->getNumberOfCellsInY(), convolutionRegularGridCompare.getNumberOfCellsInY());
    EXPECT_EQ(convolutionRegularGrid->getNumberOfCellsInZ(), convolutionRegularGridCompare.getNumberOfCellsInZ());

    for (size_t x = 0; x < convolutionRegularGrid->getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < convolutionRegularGrid->getNumberOfCellsInY(); ++y)
        {
            for (size_t z = 0; z < convolutionRegularGrid->getNumberOfCellsInZ(); ++z)
            {
                EXPECT_LT(std::abs(convolutionRegularGrid->getData(x, y, z) - convolutionRegularGridCompare.getData(x, y, z)), epsilon);
            }
        }
    }
}
