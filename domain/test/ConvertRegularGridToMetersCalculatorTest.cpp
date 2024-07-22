#include <domain/src/ConvertRegularGridCalculator.h>
#include "domain/src/ImpedanceRegularGridCalculator.h"
#include <domain/src/VolumeToRegularGrid.h>
#include <gtest/gtest.h>
#include "DomainTestValues.h"
#include "domain/src/ImpedanceRegularGridCalculator.h"
#include "domain/test/ConvertRegularGridTestValues.h"

using namespace syntheticSeismic::domain;
using namespace syntheticSeismic::geometry;


void showRegularGridVolume(RegularGrid<std::shared_ptr<Volume>> &regularGrid)
{
    std::cout << "NumberOfCellsInX: " << regularGrid.getNumberOfCellsInX() << std::endl;
    std::cout << "NumberOfCellsInY: " << regularGrid.getNumberOfCellsInY() << std::endl;
    std::cout << "NumberOfCellsInZ: " << regularGrid.getNumberOfCellsInZ() << std::endl;
    std::cout << "UnitInX: " << regularGrid.getUnitInX() << std::endl;
    std::cout << "UnitInY: " << regularGrid.getUnitInY() << std::endl;
    std::cout << "UnitInZ: " << regularGrid.getUnitInZ() << std::endl;
    std::cout << "CellSizeInX: " << regularGrid.getCellSizeInX() << std::endl;
    std::cout << "CellSizeInY: " << regularGrid.getCellSizeInY() << std::endl;
    std::cout << "CellSizeInZ: " << regularGrid.getCellSizeInZ() << std::endl;

    const auto &data = regularGrid.getData();
    for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
        {
            std::cout << x << " " << y << ": ";
            for (size_t z = 0; z < regularGrid.getNumberOfCellsInZ(); ++z)
            {
                if (data[x][y][z] == nullptr)
                {
                    std::cout << "0 ";
                }
                else
                {
                    std::cout << data[x][y][z]->indexVolume << " ";
                }
            }
            std::cout << std::endl;
        }
    }
}

void showRegularGrid(RegularGrid<double> &regularGrid)
{
    std::cout << "NumberOfCellsInX: " << regularGrid.getNumberOfCellsInX() << std::endl;
    std::cout << "NumberOfCellsInY: " << regularGrid.getNumberOfCellsInY() << std::endl;
    std::cout << "NumberOfCellsInZ: " << regularGrid.getNumberOfCellsInZ() << std::endl;
    std::cout << "UnitInX: " << regularGrid.getUnitInX() << std::endl;
    std::cout << "UnitInY: " << regularGrid.getUnitInY() << std::endl;
    std::cout << "UnitInZ: " << regularGrid.getUnitInZ() << std::endl;
    std::cout << "CellSizeInX: " << regularGrid.getCellSizeInX() << std::endl;
    std::cout << "CellSizeInY: " << regularGrid.getCellSizeInY() << std::endl;
    std::cout << "CellSizeInZ: " << regularGrid.getCellSizeInZ() << std::endl;

    const auto &data = regularGrid.getData();
    for (size_t x = 0; x < regularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < regularGrid.getNumberOfCellsInY(); ++y)
        {
            std::cout << x << " " << y << ": ";
            for (size_t z = 0; z < regularGrid.getNumberOfCellsInZ(); ++z)
            {
                std::cout << data[x][y][z] << " ";
            }
            std::cout << std::endl;
        }
    }
}

TEST(DomainTest, ConvertRegularGridFromZInSecondsToZInMeters) {
    const auto undefinedLithology = std::make_shared<Lithology>(0, "undefined", 2.500, 1);

    const std::vector<std::shared_ptr<Lithology>> lithologies = {
            std::make_shared<Lithology>(1, "mudstone", 2.800, 1),
            std::make_shared<Lithology>(3, "siltite", 3.000, 1),
            std::make_shared<Lithology>(5, "fine-grained sandstone", 3.200, 1),
            std::make_shared<Lithology>(7, "fine-grained sandstone", 3.200, 1),
            std::make_shared<Lithology>(9, "medium-grained sandstone", 3.500, 1),
            std::make_shared<Lithology>(11, "coarse-grained sandstone", 3.700, 1),
            std::make_shared<Lithology>(13, "very coarse-grained sandstone", 4.000, 1),
            std::make_shared<Lithology>(15, "conglomerate", 4.500, 1),
            std::make_shared<Lithology>(17, "conglomerate", 4.500, 1),
            std::make_shared<Lithology>(24, "volcanic", 6.000, 1)
    };

    const auto volumesResult = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    auto regularGridCompare = DomainTestValues::regularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumesResult.volumes, volumesResult.rectanglePoints, volumesResult.zBottom, volumesResult.zTop);

    ConvertRegularGridCalculator convertCalculator(undefinedLithology);
    for (const auto &lithology : lithologies) {
        convertCalculator.addLithology(lithology);
    }
    auto timeRegularGrid = convertCalculator.fromZInMetersToZInSeconds(regularGrid);

    ImpedanceRegularGridCalculator impedanceCalculator(undefinedLithology);
    for (const auto &lithology : lithologies) {
        impedanceCalculator.addLithology(lithology);
    }
    auto timeImpedanceRegularGridPtr = impedanceCalculator.execute(timeRegularGrid);
    auto timeImpedanceRegularGrid = *timeImpedanceRegularGridPtr;

    ConvertRegularGridCalculator convertCalculator2(undefinedLithology);
    for (const auto &lithology : lithologies) {
        convertCalculator2.addLithology(lithology);
    }
    auto depthImpedanceRegularGrid = convertCalculator2.fromZInSecondsToZInMeters(timeRegularGrid, timeImpedanceRegularGrid);
}


TEST(DomainTest, ConvertRegularGridFromSecondsToMetersUniDimensional) {
    auto traceInSecondsData = ConvertRegularGridTestValues::traceInTime();

    auto lithologyInSecondsData = ConvertRegularGridTestValues::lithologiesInTime();

    const auto undefinedLithology = std::make_shared<Lithology>(0, "undefined", 2.500, 1);

    const std::vector<std::shared_ptr<Lithology>> lithologies = {
        std::make_shared<Lithology>(1, "mudstone", 2.800, 1),
        std::make_shared<Lithology>(3, "siltite", 3.000, 1),
        std::make_shared<Lithology>(5, "fine-grained sandstone", 3.200, 1),
        std::make_shared<Lithology>(7, "fine-grained sandstone", 3.200, 1),
        std::make_shared<Lithology>(9, "medium-grained sandstone", 3.500, 1),
        std::make_shared<Lithology>(11, "coarse-grained sandstone", 3.700, 1),
        std::make_shared<Lithology>(13, "very coarse-grained sandstone", 4.000, 1),
        std::make_shared<Lithology>(15, "conglomerate", 4.500, 1),
        std::make_shared<Lithology>(17, "conglomerate", 4.500, 1),
        std::make_shared<Lithology>(24, "volcanic", 6.000, 1)
    };

    const auto traceInMetersResult = ConvertRegularGridTestValues::traceInDepth();

    const size_t numberOfCellsInX = 1;
    const size_t numberOfCellsInY = 1;
    const size_t numberOfCellsInZ = traceInSecondsData.size();
    const double timeStep = 1.0;

    RegularGrid<double> traceInSeconds(
        numberOfCellsInX,
        numberOfCellsInY,
        numberOfCellsInZ,
        1.0,
        1.0,
        timeStep,
        EnumUnit::Meters,
        EnumUnit::Meters,
        EnumUnit::Seconds,
        std::array<syntheticSeismic::geometry::Point2D, 4> {Point2D(0.0, 0.0), Point2D(0.0, 0.0), Point2D(0.0, 0.0), Point2D(0.0, 0.0)},
        0.0,
        0.0,
        0.0
        );

    auto traceInSecondsDataPointer = traceInSeconds.getData();
    traceInSecondsDataPointer[0][0] = ConvertRegularGridTestValues::traceInTime();

    RegularGrid<std::shared_ptr<syntheticSeismic::geometry::Volume>> lithologyInSeconds(
        numberOfCellsInX,
        numberOfCellsInY,
        numberOfCellsInZ,
        1.0,
        1.0,
        timeStep,
        EnumUnit::Meters,
        EnumUnit::Meters,
        EnumUnit::Seconds,
        std::array<syntheticSeismic::geometry::Point2D, 4> {Point2D(0.0, 0.0), Point2D(0.0, 0.0), Point2D(0.0, 0.0), Point2D(0.0, 0.0)},
        0.0,
        0.0,
        nullptr
        );

    ConvertRegularGridCalculator convertCalculator(undefinedLithology);
    for (const auto &lithology : lithologies) {
        convertCalculator.addLithology(lithology);
    }

    auto traceInMetersConverted = convertCalculator.fromZInSecondsToZInMeters(lithologyInSeconds, traceInSeconds);


    auto &data = traceInMetersConverted.getData();

    auto limit = traceInMetersConverted.getNumberOfCellsInZ();


    for (size_t i = 0; i < limit; ++i) {
        double expectedValue;
        double calculatedValue;

        expectedValue = traceInSecondsData[i];
        calculatedValue = data[0][0][i];

        EXPECT_EQ(expectedValue, calculatedValue);
    }
}
