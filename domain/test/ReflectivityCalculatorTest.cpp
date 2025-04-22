#include <gtest/gtest.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <domain/src/ReflectivityRegularGridCalculator.h>
#include <storage/src/reader/GrdSurfaceReader.h>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "DomainTestValues.h"

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