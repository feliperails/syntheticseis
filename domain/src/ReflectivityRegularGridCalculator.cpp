#include "ReflectivityRegularGridCalculator.h"
#include "Lithology.h"
#include "geometry/src/Volume.h"
#include <omp.h>

namespace syntheticSeismic {
namespace domain {

ReflectivityRegularGridCalculator::ReflectivityRegularGridCalculator(double undefinedImpedance)
    : m_undefinedImpedance(undefinedImpedance)
{

}

void ReflectivityRegularGridCalculator::convert(RegularGrid<double> &regularImpedanceGrid)
{
    const auto numberOfCellsInX = regularImpedanceGrid.getNumberOfCellsInX();
    const auto numberOfCellsInY = regularImpedanceGrid.getNumberOfCellsInY();
    const auto numberOfCellsInZ = static_cast<int>(regularImpedanceGrid.getNumberOfCellsInZ());

    RegularGrid<double> reflectivityGrid(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                regularImpedanceGrid.getCellSizeInX(), regularImpedanceGrid.getCellSizeInY(), regularImpedanceGrid.getCellSizeInZ(),
                regularImpedanceGrid.getNoDataValue()
            );
    auto &data = regularImpedanceGrid.getData();

    #pragma omp parallel for
    for (int zInt = 0; zInt < numberOfCellsInZ; ++zInt)
    {
        const auto z = static_cast<size_t>(zInt);
        for (size_t y = 0; y < numberOfCellsInY; ++y)
        {
            for (size_t x = 0; x < numberOfCellsInX; ++x)
            {
                if (z == 0)
                {
                    const auto impedance1 = data[x][y][z];
                    data[x][y][z] = (m_undefinedImpedance - impedance1) / (m_undefinedImpedance + impedance1);
                }
                else
                {
                    const auto impedance1 = data[x][y][z - 1];
                    const auto impedance2 = data[x][y][z];
                    data[x][y][z] = (impedance2 - impedance1) / (impedance2 + impedance1);
                }
            }
        }
    }
}

} // namespace domain
} // namespace syntheticSeismic
