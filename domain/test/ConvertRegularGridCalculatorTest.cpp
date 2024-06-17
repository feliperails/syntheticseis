#include <domain/src/ConvertRegularGridCalculator.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <gtest/gtest.h>
#include "DomainTestValues.h"

using namespace syntheticSeismic::domain;
using namespace syntheticSeismic::geometry;

TEST(DomainTest, ConvertRegularGrid) {
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
    std::cout << "NumberOfCellsInX: " << timeRegularGrid.getNumberOfCellsInX() << std::endl;
    std::cout << "NumberOfCellsInY: " << timeRegularGrid.getNumberOfCellsInY() << std::endl;
    std::cout << "NumberOfCellsInZ: " << timeRegularGrid.getNumberOfCellsInZ() << std::endl;
    std::cout << "UnitInX: " << timeRegularGrid.getUnitInX() << std::endl;
    std::cout << "UnitInY: " << timeRegularGrid.getUnitInY() << std::endl;
    std::cout << "UnitInZ: " << timeRegularGrid.getUnitInZ() << std::endl;
    std::cout << "CellSizeInX: " << timeRegularGrid.getCellSizeInX() << std::endl;
    std::cout << "CellSizeInY: " << timeRegularGrid.getCellSizeInY() << std::endl;
    std::cout << "CellSizeInZ: " << timeRegularGrid.getCellSizeInZ() << std::endl;

    const auto &data = timeRegularGrid.getData();
    for (size_t x = 0; x < timeRegularGrid.getNumberOfCellsInX(); ++x)
    {
        for (size_t y = 0; y < timeRegularGrid.getNumberOfCellsInY(); ++y)
        {
            std::cout << x << " " << y << ": ";
            for (size_t z = 0; z < timeRegularGrid.getNumberOfCellsInZ(); ++z)
            {
                if (data[x][y][z] == nullptr)
                {
                    std::cout << "0 ";
                }
                else
                {
                    std::cout << data[x][y][z]->idLithology << " ";
                }
            }
            std::cout << std::endl;
        }
    }
}
