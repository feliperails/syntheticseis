#include <memory>
#include "ConvertRegularGridCalculator.h"
#include "RegularGrid.h"
#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {
    ConvertRegularGridCalculator::ConvertRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology)
            : m_undefinedLithology(undefinedLithology)
    {

    }

    void ConvertRegularGridCalculator::addLithology(std::shared_ptr<Lithology> lithology)
    {
        m_lithologies[lithology->getId()] = lithology;
    }

    std::shared_ptr<RegularGrid<double>> ConvertRegularGridCalculator::fromZInMetersToZInTime(RegularGrid<std::shared_ptr<geometry::Volume>> &regularVolumeGrid)
    {
        const auto maxVelocity = computeMaxVelocity(regularVolumeGrid);

        const auto numberOfCellsInX = regularVolumeGrid.getNumberOfCellsInX();
        const auto numberOfCellsInY = regularVolumeGrid.getNumberOfCellsInY();
        const auto numberOfCellsInZ = regularVolumeGrid.getNumberOfCellsInZ();
        const auto numberOfCellsInZInt = static_cast<int>(numberOfCellsInZ);

        const auto cellSizeInZ = regularVolumeGrid.getCellSizeInZ();

        const auto timeStep = regularVolumeGrid.getCellSizeInZ() / maxVelocity;

        auto regularGrid = std::make_shared<RegularGrid<double>>(
                numberOfCellsInX,
                numberOfCellsInY,
                numberOfCellsInZ,
                regularVolumeGrid.getCellSizeInX(),
                regularVolumeGrid.getCellSizeInY(),
                timeStep,
                regularVolumeGrid.getUnitInX(),
                regularVolumeGrid.getUnitInY(),
                EnumUnit::Seconds,
                regularVolumeGrid.getRectanglePoints(),
                regularVolumeGrid.getZBottom(),
                regularVolumeGrid.getZTop(),
                m_undefinedLithology->getVelocity() * m_undefinedLithology->getDensity()
        );
        auto &data = regularGrid->getData();

        const auto &gridDataVolumes = regularVolumeGrid.getData();

        #pragma omp parallel for
        for (size_t x = 0; x < numberOfCellsInX; ++x)
        {
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                double currentTime = 0;
                int indexCell = 0;
                for (int zInt = 0; zInt < numberOfCellsInZInt; ++zInt)
                {
                    const auto z = static_cast<size_t>(zInt);
                    auto velocity = m_undefinedLithology->getVelocity();
                    if (gridDataVolumes[x][y][z] == nullptr)
                    {
                        const auto idLithology = gridDataVolumes[x][y][z]->idLithology;
                        const auto &lithology = *m_lithologies[idLithology];
                        velocity = lithology.getVelocity();
                    }

                    currentTime += cellSizeInZ / velocity;

                    const auto numberOfCellsInTimeFromDistance = currentTime / timeStep;
                    const auto limit = floor(numberOfCellsInTimeFromDistance);
                    for (auto newZ= indexCell; newZ < limit; ++newZ)
                    {
                        wellInTime(j) = lithology;
                        wellVelocitiesInTime(j) = velocity;
                    }

                    const auto idLithology = gridDataVolumes[x][y][z]->idLithology;
                    if (m_lithologies.find(idLithology) == m_lithologies.end()) {
                        QString message = "Lithology " + QString::number(gridDataVolumes[x][y][z]->idLithology) +
                                          " of volume " +
                                          QString::number(gridDataVolumes[x][y][z]->indexVolume) + " of " +
                                          QString::number(x) + ", " +
                                          QString::number(y) + " and " + QString::number(x) +
                                          " coordinates was not found.";
                        error = true;
                        exception = std::exception(message.toStdString().c_str());
                        continue;
                    }


                }
            }
        }

        return regularGrid;
    }

    double ConvertRegularGridCalculator::computeMaxVelocity(RegularGrid <std::shared_ptr<geometry::Volume>> &grid) {
        const size_t numberOfCellsInX = grid.getNumberOfCellsInX();
        const size_t numberOfCellsInY = grid.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = grid.getNumberOfCellsInZ();
        const int numberOfCellsInZInt = static_cast<int>(numberOfCellsInZ);
        const auto &gridDataVolumes= grid.getData();
        if (grid.getUnitInZ() == Meters)
        {
            throw std::exception("The Z grid unit must be in meters.");
        }
        double maxVelocity = 0.0;

        bool error = false;
        std::exception exception;

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

                    if (m_lithologies.find(idLithology) == m_lithologies.end())
                    {
                        QString message = "Lithology " + QString::number(gridDataVolumes[x][y][z]->idLithology) + " of volume " +
                                          QString::number(gridDataVolumes[x][y][z]->indexVolume) + " of " + QString::number(x) + ", " +
                                          QString::number(y) + " and " + QString::number(x) + " coordinates was not found.";
                        error = true;
                        exception = std::exception(message.toStdString().c_str());
                        continue;
                    }

                    const auto &lithology = *m_lithologies[idLithology];
                    if (maxVelocity < lithology.getVelocity())
                    {
                        maxVelocity = lithology.getVelocity();
                    }
                }
            }
        }
        if (error)
        {
            throw exception;
        }

        return maxVelocity;
    }
} // domain
} // syntheticSeismic
