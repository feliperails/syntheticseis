#include <gtest/gtest.h>
#include <domain/src/ConvolutionRegularGridCalculator.h>
#include "DomainTestValues.h"

using namespace syntheticSeismic::domain;

TEST(DomainTest, Convolution) {
    return;

    auto regularGrid = DomainTestValues::regularGridToTestConvolution();
    auto wavelet = DomainTestValues::waveletToTestConvolution();
    const auto convolutionRegularGridCompare = DomainTestValues::regularGridConvolution();
    const double epsilon = std::pow(10, -10);

    ConvolutionRegularGridCalculator convolutionCalculator;
    auto convolutionRegularGrid = convolutionCalculator.execute(regularGrid, wavelet);

    EXPECT_EQ(convolutionRegularGrid->getNumberOfCellsInX(), convolutionRegularGridCompare.getNumberOfCellsInX());
    EXPECT_EQ(convolutionRegularGrid->getNumberOfCellsInY(), convolutionRegularGridCompare.getNumberOfCellsInY());
    EXPECT_EQ(convolutionRegularGrid->getNumberOfCellsInZ(), convolutionRegularGridCompare.getNumberOfCellsInZ());

    for (size_t x = 0; x < convolutionRegularGrid->getNumberOfCellsInX(); ++x) {
        for (size_t y = 0; y < convolutionRegularGrid->getNumberOfCellsInY(); ++y) {
            for (size_t z = 0; z < convolutionRegularGrid->getNumberOfCellsInZ(); ++z) {
                EXPECT_LT(std::abs(
                        convolutionRegularGrid->getData(x, y, z) - convolutionRegularGridCompare.getData(x, y, z)),
                          epsilon);
            }
        }
    }
}