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