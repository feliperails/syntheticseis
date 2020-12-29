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

std::shared_ptr<RegularGrid<double>> ReflectivityRegularGridCalculator::execute(RegularGrid<double> &regularImpedanceGrid)
{
    const auto numberOfCellsInX = regularImpedanceGrid.getNumberOfCellsInX();
    const auto numberOfCellsInY = regularImpedanceGrid.getNumberOfCellsInY();
    const auto numberOfCellsInZ = static_cast<int>(regularImpedanceGrid.getNumberOfCellsInZ());

    auto regularGrid = std::make_shared<RegularGrid<double>>(
                               numberOfCellsInX,
                               numberOfCellsInY,
                               numberOfCellsInZ,
                               regularImpedanceGrid.getCellSizeInX(),
                               regularImpedanceGrid.getCellSizeInY(),
                               regularImpedanceGrid.getCellSizeInZ(),
                               0, 0
                            );

    auto &dataImpedance = regularImpedanceGrid.getData();
    auto &data = regularGrid->getData();

    #pragma omp parallel for
    for (int zInt = 0; zInt < numberOfCellsInZ; ++zInt)
    {
        const auto z = static_cast<size_t>(zInt);
        for (size_t y = 0; y < numberOfCellsInY; ++y)
        {
            for (size_t x = 0; x < numberOfCellsInX; ++x)
            {
                const auto impedance1 = dataImpedance[x][y][z];
                if (z == 0)
                {
                    data[x][y][z] = (m_undefinedImpedance - impedance1) / (m_undefinedImpedance + impedance1);
                }
                else
                {
                    const auto impedance2 = dataImpedance[x][y][z - 1];
                    data[x][y][z] = (impedance2 - impedance1) / (impedance2 + impedance1);
                }
            }
        }
    }

    return regularGrid;
}

} // namespace domain
} // namespace syntheticSeismic
