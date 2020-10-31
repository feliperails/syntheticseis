#include "ImpedanceRegularGridCalculator.h"
#include "Lithology.h"
#include "geometry/src/Volume.h"

namespace syntheticSeismic {
namespace domain {

ImpedanceRegularGridCalculator::ImpedanceRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology)
    : m_undefinedLithology(undefinedLithology)
{

}

void ImpedanceRegularGridCalculator::addLithology(std::shared_ptr<Lithology> lithology)
{
    m_lithologies[lithology->getId()] = lithology;
}

std::shared_ptr<RegularGrid<double>> ImpedanceRegularGridCalculator::convert(RegularGrid<std::shared_ptr<geometry::Volume>> regularVolumeGrid)
{
    const auto numberOfCellsInX = regularVolumeGrid.getNumberOfCellsInX();
    const auto numberOfCellsInY = regularVolumeGrid.getNumberOfCellsInY();
    const auto numberOfCellsInZ = regularVolumeGrid.getNumberOfCellsInZ();
    const auto numberOfCellsInZInt = static_cast<int>(numberOfCellsInZ);

    auto regularGrid = std::make_shared<RegularGrid<double>>(
                               numberOfCellsInX,
                               numberOfCellsInY,
                               numberOfCellsInZ,
                               regularVolumeGrid.getCellSizeInX(),
                               regularVolumeGrid.getCellSizeInY(),
                               regularVolumeGrid.getCellSizeInZ(),
                               m_undefinedLithology->getVelocity() * m_undefinedLithology->getDensity()
                            );
    auto &data = regularGrid->getData();

    const auto &gridDataVolumes = regularVolumeGrid.getData();

    #pragma omp parallel for
    for (int zInt = 0; zInt < numberOfCellsInZInt; ++zInt)
    {
        const auto z = static_cast<size_t>(zInt);
        for (size_t y = 0; y < numberOfCellsInY; ++y)
        {
            for (size_t x = 0; x < numberOfCellsInX; ++x)
            {
                if (gridDataVolumes[x][y][z] == nullptr)
                {
                    continue;
                }

                const auto idLithology = gridDataVolumes[x][y][z]->idLithology;
                const auto &lithology = *m_lithologies[idLithology];
                data[x][y][z] = lithology.getVelocity() * lithology.getDensity();
            }
        }
    }

    return regularGrid;
}

} // namespace domain
} // namespace syntheticSeismic
