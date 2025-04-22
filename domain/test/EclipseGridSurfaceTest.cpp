#include <gtest/gtest.h>
#include <domain/src/EclipseGridSurface.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <storage/src/reader/GrdSurfaceReader.h>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "DomainTestValues.h"

TEST(DomainTest, EclipseGridSurface)
{
    using namespace syntheticSeismic::domain;

    const auto simpleGrid = DomainTestValues::eclipseGridFromSimpleGrid();
    auto compare = DomainTestValues::simpleGrdSurfaceResult();
    auto resultCompare = std::get<0>(compare);
    const double epsilon = std::pow(10, -10);

    EclipseGridSurface eclipseGridSurface(simpleGrid, 5, 5);
    auto result = eclipseGridSurface.extractMainSurface();
    EXPECT_LT(std::abs(result->getSurface()->getXMin() - resultCompare.getSurface()->getXMin()), epsilon);
    EXPECT_LT(std::abs(result->getSurface()->getYMin() - resultCompare.getSurface()->getYMin()), epsilon);
    EXPECT_LT(std::abs(result->getSurface()->getXMax() - resultCompare.getSurface()->getXMax()), epsilon);
    EXPECT_LT(std::abs(result->getSurface()->getYMax() - resultCompare.getSurface()->getYMax()), epsilon);
    EXPECT_LT(std::abs(result->getSurface()->getZMin() - std::get<1>(compare)), epsilon);
    EXPECT_LT(std::abs(result->getSurface()->getZMax() - std::get<2>(compare)), epsilon);

    EXPECT_LT(std::abs(result->getLithologySurface()->getXMin() - resultCompare.getLithologySurface()->getXMin()), epsilon);
    EXPECT_LT(std::abs(result->getLithologySurface()->getYMin() - resultCompare.getLithologySurface()->getYMin()), epsilon);
    EXPECT_LT(std::abs(result->getLithologySurface()->getXMax() - resultCompare.getLithologySurface()->getXMax()), epsilon);
    EXPECT_LT(std::abs(result->getLithologySurface()->getYMax() - resultCompare.getLithologySurface()->getYMax()), epsilon);
    EXPECT_LT(std::abs(result->getLithologySurface()->getZMin() - std::get<3>(compare)), epsilon);
    EXPECT_LT(std::abs(result->getLithologySurface()->getZMax() - std::get<4>(compare)), epsilon);

    EXPECT_LT(std::abs(result->getFaciesAssociationSurface()->getXMin() - resultCompare.getFaciesAssociationSurface()->getXMin()), epsilon);
    EXPECT_LT(std::abs(result->getFaciesAssociationSurface()->getYMin() - resultCompare.getFaciesAssociationSurface()->getYMin()), epsilon);
    EXPECT_LT(std::abs(result->getFaciesAssociationSurface()->getXMax() - resultCompare.getFaciesAssociationSurface()->getXMax()), epsilon);
    EXPECT_LT(std::abs(result->getFaciesAssociationSurface()->getYMax() - resultCompare.getFaciesAssociationSurface()->getYMax()), epsilon);
    EXPECT_LT(std::abs(result->getFaciesAssociationSurface()->getZMin() - std::get<5>(compare)), epsilon);
    EXPECT_LT(std::abs(result->getFaciesAssociationSurface()->getZMax() - std::get<6>(compare)), epsilon);

    for (size_t i = 0; i < result->getSurface()->getData().size(); ++i)
    {
        for (size_t j = 0; j < result->getSurface()->getData()[0].size(); ++j)
        {
            EXPECT_LT(std::abs(result->getSurface()->getData()[i][j] - resultCompare.getSurface()->getData()[i][j]), epsilon);
            EXPECT_EQ(result->getLithologySurface()->getData()[i][j], resultCompare.getLithologySurface()->getData()[i][j]);
            EXPECT_EQ(result->getFaciesAssociationSurface()->getData()[i][j], resultCompare.getFaciesAssociationSurface()->getData()[i][j]);
        }
    }
}
