#include <gtest/gtest.h>
#include <domain/src/Lithology.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <domain/src/VolumeToRegularGrid.h>
#include <storage/src/reader/GrdSurfaceReader.h>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "DomainTestValues.h"

TEST(DomainTest, ImpedanceCalculator)
{
    using namespace syntheticSeismic::domain;
    using namespace syntheticSeismic::geometry;

    const auto volumesResult = DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees();
    const auto impedanceRegularGridCompare = DomainTestValues::impedanceRegularGridFromSimpleGridRotated30Degrees();

    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;
    double epsilon = std::pow(10, -10);

    VolumeToRegularGrid volumeToRegularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ);
    auto regularGrid = volumeToRegularGrid.convertVolumesToRegularGrid(volumesResult.volumes, volumesResult.rectanglePoints, volumesResult.zBottom, volumesResult.zTop);

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