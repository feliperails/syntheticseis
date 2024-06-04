#include "ConvolutionRegularGridCalculator.h"

#include <iostream>

#define PROCESS_TRACE_CHUNK 100

namespace syntheticSeismic {
namespace domain {

ConvolutionRegularGridCalculator::ConvolutionRegularGridCalculator() = default;

std::shared_ptr<RegularGrid<double>> ConvolutionRegularGridCalculator::execute(RegularGrid<double> &reflectivityRegularGrid, Wavelet &wavelet)
{
    const auto reflectivitySize = reflectivityRegularGrid.getNumberOfCellsInZ();
    const auto &waveletTraces = wavelet.getTraces();
    const auto waveletSize = waveletTraces.size();
    const auto numberOfCellsInX = reflectivityRegularGrid.getNumberOfCellsInX();
    const auto numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);
    const auto numberOfCellsInY = reflectivityRegularGrid.getNumberOfCellsInY();
    const auto numberOfCellsInZ = reflectivitySize + waveletSize - 1;
    const double defaultValue = 0.0;
    const int noDataValue = 0;

    auto regularGrid = std::make_shared<RegularGrid<double>>(
                               numberOfCellsInX,
                               numberOfCellsInY,
                               numberOfCellsInZ,
                               reflectivityRegularGrid.getCellSizeInX(),
                               reflectivityRegularGrid.getCellSizeInY(),
                               static_cast<double>(reflectivitySize) / static_cast<double>(numberOfCellsInZ) * reflectivityRegularGrid.getCellSizeInZ(),
                               EnumUnit::Meters, EnumUnit::Meters, EnumUnit::Meters,
                               reflectivityRegularGrid.getRectanglePoints(),
                               reflectivityRegularGrid.getZBottom(),
                               reflectivityRegularGrid.getZTop(),
                               defaultValue,
                               noDataValue
                            );
    const auto &impedanceData = reflectivityRegularGrid.getData();
    auto &data = regularGrid->getData();

    #pragma omp parallel for schedule(dynamic, PROCESS_TRACE_CHUNK)
    for (int xInt = 0; xInt < numberOfCellsInXInt; ++xInt)
    {
        const auto x = static_cast<size_t>(xInt);
        for (size_t y = 0; y < numberOfCellsInY; ++y)
        {
            for (size_t z = 0; z < numberOfCellsInZ; ++z)
            {
                const size_t jmn = (z >= waveletSize - 1) ? z - (waveletSize - 1) : 0;
                const size_t jmx = (z < reflectivitySize - 1) ? z : reflectivitySize - 1;
                for (auto j = jmn; j <= jmx; ++j)
                {
                    data[x][y][z] += (impedanceData[x][y][j] * waveletTraces[z - j]);
                }
            }
        }
    }

    auto regularGridWithoutBorders = std::make_shared<RegularGrid<double>>(
                               numberOfCellsInX,
                               numberOfCellsInY,
                               reflectivitySize,
                               reflectivityRegularGrid.getCellSizeInX(),
                               reflectivityRegularGrid.getCellSizeInY(),
                               reflectivityRegularGrid.getCellSizeInZ(),
                               reflectivityRegularGrid.getUnitInX(),
                               reflectivityRegularGrid.getUnitInY(),
                               reflectivityRegularGrid.getUnitInZ(),
                               reflectivityRegularGrid.getRectanglePoints(),
                               reflectivityRegularGrid.getZBottom(),
                               reflectivityRegularGrid.getZTop(),
                               defaultValue,
                               noDataValue
                            );
    auto &dataWithoutBorders = regularGridWithoutBorders->getData();
    const auto startZ = static_cast<size_t>((numberOfCellsInZ - reflectivitySize) / 2.0);
    for (size_t x = 0; x < numberOfCellsInX; ++x)
    {
        for (size_t y = 0; y < numberOfCellsInY; ++y)
        {
            for (size_t z = 0; z < reflectivitySize; ++z)
            {
                dataWithoutBorders[x][y][z] = data[x][y][startZ + z];
            }
        }
    }

    return regularGridWithoutBorders;
}

} // namespace domain
} // namespace syntheticSeismic
