#include "ConvolutionRegularGridCalculator.h"

#include <iostream>

#define PROCESS_TRACE_CHUNK 100

namespace syntheticSeismic {
namespace domain {

ConvolutionRegularGridCalculator::ConvolutionRegularGridCalculator()
{

}

std::shared_ptr<RegularGrid<double>> ConvolutionRegularGridCalculator::execute(RegularGrid<double> &impedanceRegularGrid, Wavelet &wavelet)
{
    const auto impedanceSize = impedanceRegularGrid.getNumberOfCellsInZ();
    const auto &waveletTraces = wavelet.getTraces();
    const auto waveletSize = waveletTraces.size();
    const auto numberOfCellsInX = impedanceRegularGrid.getNumberOfCellsInX();
    const auto numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);
    const auto numberOfCellsInY = impedanceRegularGrid.getNumberOfCellsInY();
    const auto numberOfCellsInZ = impedanceSize + waveletSize - 1;
    const double defaultValue = 0.0;
    const int noDataValue = 0;

    auto regularGrid = std::make_shared<RegularGrid<double>>(
                               numberOfCellsInX,
                               numberOfCellsInY,
                               numberOfCellsInZ,
                               impedanceRegularGrid.getCellSizeInX(),
                               impedanceRegularGrid.getCellSizeInY(),
                               impedanceRegularGrid.getCellSizeInZ(),
                               defaultValue,
                               noDataValue
                            );
    const auto &impedanceData = impedanceRegularGrid.getData();
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
                const size_t jmx = (z < impedanceSize - 1) ? z : impedanceSize - 1;
                for (auto j = jmn; j <= jmx; ++j)
                {
                    data[x][y][z] += (impedanceData[x][y][j] * waveletTraces[z - j]);
                }
            }
        }
    }

    return regularGrid;
}

} // namespace domain
} // namespace syntheticSeismic
