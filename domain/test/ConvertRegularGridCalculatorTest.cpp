#include <domain/src/ConvertRegularGridCalculator.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <gtest/gtest.h>
#include "DomainTestValues.h"
#include "ConvertRegularGridTestValues.h"

using namespace syntheticSeismic::domain;
using namespace syntheticSeismic::geometry;

TEST(DomainTest, ConvertRegularGridFromZInMetersToZInSecondsUniDimensional) {
    auto regularGrid = ConvertRegularGridTestValues::regularGridLithologyOneDimensionalDepth();

    ConvertRegularGridCalculator convertCalculator(std::make_shared<Lithology>(0, "undefined", 2.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(1, "mudstone", 2.800, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(2, "siltite", 3.000, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(3, "fine-grained sandstone", 3.200, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(4, "medium-grained sandstone", 3.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(5, "coarse-grained sandstone", 3.700, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(6, "very coarse-grained sandstone", 4.000, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(7, "conglomerate", 4.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(8, "volcanic", 6.000, 1));
    auto timeRegularGrid = convertCalculator.fromZInMetersToZInSeconds(regularGrid);
    auto &data = timeRegularGrid.getData();

    auto limit = timeRegularGrid.getNumberOfCellsInZ();

    auto lithologiesInTime = ConvertRegularGridTestValues::lithologiesInTime();

    for (size_t i = 0; i < limit; ++i) {
        auto volume = data[0][0][i];
        auto lithology = volume == nullptr ? -1 : data[0][0][i]->idLithology;
        EXPECT_EQ(lithology, lithologiesInTime[i]);
    }
}

TEST(DomainTest, ConvertRegularGridFromZInMetersToZInSeconds) {
    const double epsilon = std::pow(10, -3);

    const auto volumesResult = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    auto regularGridCompare = DomainTestValues::regularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumesResult.volumes, volumesResult.rectanglePoints, volumesResult.zBottom, volumesResult.zTop);

    ConvertRegularGridCalculator convertCalculator(std::make_shared<Lithology>(0, "undefined", 2.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(1, "mudstone", 2.800, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(3, "siltite", 3.000, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(5, "fine-grained sandstone", 3.200, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(7, "fine-grained sandstone", 3.200, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(9, "medium-grained sandstone", 3.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(11, "coarse-grained sandstone", 3.700, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(13, "very coarse-grained sandstone", 4.000, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(15, "conglomerate", 4.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(17, "conglomerate", 4.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(24, "volcanic", 6.000, 1));
    auto timeRegularGrid = convertCalculator.fromZInMetersToZInSeconds(regularGrid);
    EXPECT_EQ(timeRegularGrid.getNumberOfCellsInX(), 5);
    EXPECT_EQ(timeRegularGrid.getNumberOfCellsInY(), 5);
    EXPECT_EQ(timeRegularGrid.getNumberOfCellsInZ(), 12);
    EXPECT_EQ(timeRegularGrid.getUnitInX(), EnumUnit::Meters);
    EXPECT_EQ(timeRegularGrid.getUnitInY(), EnumUnit::Meters);
    EXPECT_EQ(timeRegularGrid.getUnitInZ(), EnumUnit::Seconds);
    EXPECT_LT(std::abs(240.0000 - timeRegularGrid.getCellSizeInX()), epsilon);
    EXPECT_LT(std::abs(160.0000 - timeRegularGrid.getCellSizeInY()), epsilon);
    EXPECT_LT(std::abs(13.3333 - timeRegularGrid.getCellSizeInZ()), epsilon);

    std::unordered_map<size_t, size_t> lithologiesMap;
    for (size_t x = 0; x < timeRegularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < timeRegularGrid.getNumberOfCellsInY(); ++y)
        {
            for (size_t z = 0; z < timeRegularGrid.getNumberOfCellsInZ(); ++z)
            {
                const auto volume = timeRegularGrid.getData()[x][y][z];
                const size_t indexVolume = volume == nullptr ? 0 : volume->indexVolume;
                if (lithologiesMap.count(indexVolume) > 0) {
                    lithologiesMap[indexVolume] = lithologiesMap[indexVolume] + 1;
                } else {
                    lithologiesMap[indexVolume] = 1;
                }
            }
        }
    }

    EXPECT_EQ(lithologiesMap[0], 189);
    EXPECT_EQ(lithologiesMap[1], 2);
    EXPECT_EQ(lithologiesMap[2], 12);
    EXPECT_EQ(lithologiesMap[3], 6);
    EXPECT_EQ(lithologiesMap[4], 10);
    EXPECT_EQ(lithologiesMap[5], 6);
    EXPECT_EQ(lithologiesMap[6], 2);
    EXPECT_EQ(lithologiesMap[7], 2);
    EXPECT_EQ(lithologiesMap[8], 2);
    EXPECT_EQ(lithologiesMap[9], 3);
    EXPECT_EQ(lithologiesMap[10], 10);
    EXPECT_EQ(lithologiesMap[11], 10);
    EXPECT_EQ(lithologiesMap[12], 2);
    EXPECT_EQ(lithologiesMap[14], 3);
    EXPECT_EQ(lithologiesMap[15], 5);
    EXPECT_EQ(lithologiesMap[16], 4);
    EXPECT_EQ(lithologiesMap[17], 8);
    EXPECT_EQ(lithologiesMap[18], 1);
    EXPECT_EQ(lithologiesMap[19], 1);
    EXPECT_EQ(lithologiesMap[20], 10);
    EXPECT_EQ(lithologiesMap[21], 2);
    EXPECT_EQ(lithologiesMap[22], 8);
    EXPECT_EQ(lithologiesMap[23], 2);

    EXPECT_EQ(lithologiesMap.size(), 23);
}

TEST(DomainTest, ConvertRegularGridFromZInMetersToZInSecondsWithWrongLithologies) {
    const double epsilon = std::pow(10, -3);

    const auto volumesResult = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    volumesResult.volumes[0]->idLithology = 0;
    auto regularGridCompare = DomainTestValues::regularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumesResult.volumes, volumesResult.rectanglePoints, volumesResult.zBottom, volumesResult.zTop);

    ConvertRegularGridCalculator convertCalculator(std::make_shared<Lithology>(0, "undefined", 2.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(1, "mudstone", 2.800, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(3, "siltite", 3.000, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(5, "fine-grained sandstone", 3.200, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(7, "fine-grained sandstone", 3.200, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(9, "medium-grained sandstone", 3.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(11, "coarse-grained sandstone", 3.700, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(13, "very coarse-grained sandstone", 4.000, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(15, "conglomerate", 4.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(17, "conglomerate", 4.500, 1));
    convertCalculator.addLithology(std::make_shared<Lithology>(24, "volcanic", 6.000, 1));
    auto timeRegularGrid = convertCalculator.fromZInMetersToZInSeconds(regularGrid);
    EXPECT_EQ(timeRegularGrid.getNumberOfCellsInX(), 5);
    EXPECT_EQ(timeRegularGrid.getNumberOfCellsInY(), 5);
    EXPECT_EQ(timeRegularGrid.getNumberOfCellsInZ(), 12);
    EXPECT_EQ(timeRegularGrid.getUnitInX(), EnumUnit::Meters);
    EXPECT_EQ(timeRegularGrid.getUnitInY(), EnumUnit::Meters);
    EXPECT_EQ(timeRegularGrid.getUnitInZ(), EnumUnit::Seconds);
    EXPECT_LT(std::abs(240.0000 - timeRegularGrid.getCellSizeInX()), epsilon);
    EXPECT_LT(std::abs(160.0000 - timeRegularGrid.getCellSizeInY()), epsilon);
    EXPECT_LT(std::abs(13.3333 - timeRegularGrid.getCellSizeInZ()), epsilon);

    std::unordered_map<size_t, size_t> lithologiesMap;
    for (size_t x = 0; x < timeRegularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < timeRegularGrid.getNumberOfCellsInY(); ++y)
        {
            for (size_t z = 0; z < timeRegularGrid.getNumberOfCellsInZ(); ++z)
            {
                const auto volume = timeRegularGrid.getData()[x][y][z];
                const size_t indexVolume = volume == nullptr ? 0 : volume->indexVolume;
                if (lithologiesMap.count(indexVolume) > 0) {
                    lithologiesMap[indexVolume] = lithologiesMap[indexVolume] + 1;
                } else {
                    lithologiesMap[indexVolume] = 1;
                }
            }
        }
    }

    EXPECT_EQ(lithologiesMap[0], 189);
    EXPECT_EQ(lithologiesMap[1], 2);
    EXPECT_EQ(lithologiesMap[2], 12);
    EXPECT_EQ(lithologiesMap[3], 6);
    EXPECT_EQ(lithologiesMap[4], 10);
    EXPECT_EQ(lithologiesMap[5], 6);
    EXPECT_EQ(lithologiesMap[6], 2);
    EXPECT_EQ(lithologiesMap[7], 2);
    EXPECT_EQ(lithologiesMap[8], 2);
    EXPECT_EQ(lithologiesMap[9], 3);
    EXPECT_EQ(lithologiesMap[10], 10);
    EXPECT_EQ(lithologiesMap[11], 10);
    EXPECT_EQ(lithologiesMap[12], 2);
    EXPECT_EQ(lithologiesMap[14], 3);
    EXPECT_EQ(lithologiesMap[15], 5);
    EXPECT_EQ(lithologiesMap[16], 4);
    EXPECT_EQ(lithologiesMap[17], 8);
    EXPECT_EQ(lithologiesMap[18], 1);
    EXPECT_EQ(lithologiesMap[19], 1);
    EXPECT_EQ(lithologiesMap[20], 10);
    EXPECT_EQ(lithologiesMap[21], 2);
    EXPECT_EQ(lithologiesMap[22], 8);
    EXPECT_EQ(lithologiesMap[23], 2);

    EXPECT_EQ(lithologiesMap.size(), 23);
}
