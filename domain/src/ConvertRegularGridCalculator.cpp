#include <cmath>
#include <memory>
#include <utility>
#include "ConvertRegularGridCalculator.h"
#include "RegularGrid.h"
#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {
    ConvertRegularGridCalculator::ConvertRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology)
            : m_undefinedLithology(std::move(undefinedLithology))
    {

    }

    void ConvertRegularGridCalculator::addLithology(const std::shared_ptr<Lithology>& lithology)
    {
        m_lithologies[lithology->getId()] = lithology;
    }

    RegularGrid<std::shared_ptr<geometry::Volume>> ConvertRegularGridCalculator::fromZInMetersToZInSeconds(RegularGrid<std::shared_ptr<geometry::Volume>> &depthGrid)
    {
        if (depthGrid.getUnitInZ() != Meters)
        {
            throw std::exception("The Z grid unit must be in meters.");
        }
        double maxVelocity;
        double maxElapsedTime;
        std::tie(maxVelocity, maxElapsedTime) = computeMaxVelocityAndElapsedTime(depthGrid);

        const auto numberOfCellsInX = depthGrid.getNumberOfCellsInX();
        const auto numberOfCellsInY = depthGrid.getNumberOfCellsInY();
        const auto numberOfCellsInZ = depthGrid.getNumberOfCellsInZ();
        const int numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);

        const auto cellSizeInZ = depthGrid.getCellSizeInZ();

        const auto timeStep = depthGrid.getCellSizeInZ() / maxVelocity;

        const auto timeNumberOfCellsInZ = static_cast<size_t>(std::ceil(maxElapsedTime / timeStep));

        RegularGrid<std::shared_ptr<geometry::Volume>> timeGrid(
                numberOfCellsInX,
                numberOfCellsInY,
                timeNumberOfCellsInZ,
                depthGrid.getCellSizeInX(),
                depthGrid.getCellSizeInY(),
                timeStep,
                depthGrid.getUnitInX(),
                depthGrid.getUnitInY(),
                EnumUnit::Seconds,
                depthGrid.getRectanglePoints(),
                depthGrid.getZBottom(),
                depthGrid.getZTop(),
                nullptr
        );
        auto &timeData = timeGrid.getData();

        const auto &metersData = depthGrid.getData();

        #pragma omp parallel for
        for (int xInt = 0; xInt < numberOfCellsInXInt; ++xInt)
        {
            const auto x = static_cast<size_t>(xInt);
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                double currentTime = 0;
                size_t indexCell = 0;
                for (size_t z = 0; z < numberOfCellsInZ; ++z)
                {
                    const auto &content = metersData[x][y][z];

                    auto velocity = m_undefinedLithology->getVelocity();
                    if (metersData[x][y][z] != nullptr)
                    {
                        const auto idLithology = metersData[x][y][z]->idLithology;
                        const auto &lithology = *m_lithologies[idLithology];
                        velocity = lithology.getVelocity();
                    }

                    currentTime += cellSizeInZ / velocity;

                    const auto numberOfCellsInTimeFromDistance = currentTime / timeStep;
                    const auto limit = static_cast<size_t>(std::round(numberOfCellsInTimeFromDistance));
                    for (auto newZ = indexCell; newZ < limit; ++newZ)
                    {
                        timeData[x][y][newZ] = content;
                    }
                    indexCell = limit;
                }
            }
        }

        return timeGrid;
    }

    RegularGrid<double>
    ConvertRegularGridCalculator::fromZInSecondsToZInMeters(RegularGrid<std::shared_ptr<geometry::Volume>>
                                                                &timeGridLithology,
                                                            RegularGrid<double> &timeGridTrace)
    {
        // Implementar aqui Carine
        const size_t numberOfCellsInX = timeGridLithology.getNumberOfCellsInX();
        const size_t numberOfCellsInY = timeGridLithology.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = timeGridLithology.getNumberOfCellsInZ();
        const auto &timeData = timeGridLithology.getData();
        const auto &timeTraceData = timeGridTrace.getData();

        const auto timeStep = timeGridLithology.getCellSizeInZ();

        RegularGrid<double> depthGrid(
            numberOfCellsInX,
            numberOfCellsInY,
            0,
            timeGridLithology.getCellSizeInX(),
            timeGridLithology.getCellSizeInY(),
            timeStep,
            timeGridLithology.getUnitInX(),
            timeGridLithology.getUnitInY(),
            EnumUnit::Meters,
            timeGridLithology.getRectanglePoints(),
            timeGridLithology.getZBottom(),
            timeGridLithology.getZTop(),
            0.0
            );

        auto &depthData = depthGrid.getData();
        double minVelocity = computeMinVelocity(timeGridLithology);

        #pragma omp parallel for
        for (int xInt = 0; xInt < static_cast<int>(numberOfCellsInX); ++xInt)
        {
            const auto x = static_cast<size_t>(xInt);
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                double currentPosition = 0.0;
                std::vector<double> positionFromTime(0);

                // Calculating position for each time slot
                for (size_t z = 0; z < numberOfCellsInZ; ++z)
                {
                    auto velocity = m_undefinedLithology->getVelocity();
                    if (timeData[x][y][z] != nullptr)
                    {
                        const auto idLithology = timeData[x][y][z]->idLithology;
                        const auto &lithology = *m_lithologies[idLithology];
                        velocity = lithology.getVelocity();
                    }
                    currentPosition += timeStep * velocity;
                    positionFromTime.push_back(currentPosition);

                }
                // Making the seismic trace equally splited in space to store in structure
                const double finalPosition = currentPosition;
                const double positionStep = timeStep * minVelocity;
                const size_t numPositionSteps = static_cast<size_t>(finalPosition/positionStep);

                double seismic;

                double position = 0.0;
                depthData[x][y].resize(numPositionSteps-1);
                for(size_t depth_idx = 0; depth_idx < numPositionSteps-2; depth_idx++)
                {
                    position = depth_idx * positionStep;
                    size_t idxB4 = 0;
                    for(size_t idx = 1; idx < numberOfCellsInZ; idx++)
                    {
                        if(positionFromTime[idx] < position)
                        {
                            idxB4 = idx;
                        }

                    }
                    seismic = (timeTraceData[x][y][idxB4] * (position - positionFromTime[idxB4]) /
                                      (positionFromTime[idxB4+1] - positionFromTime[idxB4])
                                      + timeTraceData[x][y][idxB4 + 1] * (positionFromTime[idxB4 + 1] - position) /
                                      (positionFromTime[idxB4+1] - positionFromTime[idxB4]));
                    depthData[x][y][depth_idx] = seismic;
                }
            }
        }
        return depthGrid;
    }

    std::pair<double, double> ConvertRegularGridCalculator::computeMaxVelocityAndElapsedTime(RegularGrid <std::shared_ptr<geometry::Volume>> &metersGrid) {
        const size_t numberOfCellsInX = metersGrid.getNumberOfCellsInX();
        const size_t numberOfCellsInY = metersGrid.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = metersGrid.getNumberOfCellsInZ();
        const int numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);
        const auto &metersData = metersGrid.getData();

        const auto cellSizeInZ = metersGrid.getCellSizeInZ();

        bool error = false;
        std::exception exception;

        std::vector<std::vector<double>> maxVelocities(numberOfCellsInX, std::vector<double>(numberOfCellsInY, 0.0));
        std::vector<std::vector<double>> elapsedTimes(numberOfCellsInX, std::vector<double>(numberOfCellsInY, 0.0));

        #pragma omp parallel for
        for (int xInt = 0; xInt < numberOfCellsInXInt; ++xInt)
        {
            const auto x = static_cast<size_t>(xInt);
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                for (size_t z = 0; z < numberOfCellsInZ; ++z)
                {
                    double velocity;
                    if (metersData[x][y][z] == nullptr)
                    {
                        velocity = m_undefinedLithology->getVelocity();
                    }
                    else
                    {
                        const auto idLithology = metersData[x][y][z]->idLithology;

                        if (m_lithologies.find(idLithology) == m_lithologies.end())
                        {
                            QString message = "Lithology " + QString::number(metersData[x][y][z]->idLithology) + " of volume " +
                                              QString::number(metersData[x][y][z]->indexVolume) + " of " + QString::number(x) + ", " +
                                              QString::number(y) + " and " + QString::number(x) + " coordinates was not found.";
                            error = true;
                            exception = std::exception(message.toStdString().c_str());
                            continue;
                        }

                        const auto &lithology = *m_lithologies[idLithology];
                        velocity = lithology.getVelocity();
                    }

                    if (maxVelocities[x][y] < velocity)
                    {
                        maxVelocities[x][y] = velocity;
                    }

                    elapsedTimes[x][y] += cellSizeInZ / velocity;
                }
            }
        }
        if (error)
        {
            throw exception;
        }

        double maxVelocity = 0.0;
        double elapsedTime = 0.0;
        for (size_t x = 0; x < numberOfCellsInX; ++x) {
            for (size_t y = 0; y < numberOfCellsInY; ++y) {
                if (maxVelocity < maxVelocities[x][y])
                {
                    maxVelocity = maxVelocities[x][y];
                }
                if (elapsedTime < elapsedTimes[x][y])
                {
                    elapsedTime = elapsedTimes[x][y];
                }
            }
        }

        return std::make_pair(maxVelocity, elapsedTime);
    }

    double ConvertRegularGridCalculator::computeMinVelocity(RegularGrid <std::shared_ptr<geometry::Volume>> &metersGrid) {
        const size_t numberOfCellsInX = metersGrid.getNumberOfCellsInX();
        const size_t numberOfCellsInY = metersGrid.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = metersGrid.getNumberOfCellsInZ();
        const int numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);
        const auto &metersData = metersGrid.getData();

        //const auto cellSizeInZ = metersGrid.getCellSizeInZ();

        bool error = false;
        std::exception exception;

        std::vector<std::vector<double>> minVelocities(numberOfCellsInX, std::vector<double>(numberOfCellsInY, 0.0));

        #pragma omp parallel for
        for (int xInt = 0; xInt < numberOfCellsInXInt; ++xInt)
        {
            const auto x = static_cast<size_t>(xInt);
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                for (size_t z = 0; z < numberOfCellsInZ; ++z)
                {
                    double velocity;
                    if (metersData[x][y][z] == nullptr)
                    {
                        velocity = m_undefinedLithology->getVelocity();
                    }
                    else
                    {
                        const auto idLithology = metersData[x][y][z]->idLithology;

                        if (m_lithologies.find(idLithology) == m_lithologies.end())
                        {
                            QString message = "Lithology " + QString::number(metersData[x][y][z]->idLithology) + " of volume " +
                                              QString::number(metersData[x][y][z]->indexVolume) + " of " + QString::number(x) + ", " +
                                              QString::number(y) + " and " + QString::number(x) + " coordinates was not found.";
                            error = true;
                            exception = std::exception(message.toStdString().c_str());
                            continue;
                        }

                        const auto &lithology = *m_lithologies[idLithology];
                        velocity = lithology.getVelocity();
                    }

                    if (minVelocities[x][y] > velocity)
                    {
                        minVelocities[x][y] = velocity;
                    }
                }
            }
        }
        if (error)
        {
            throw exception;
        }

        double minVelocity = 0.0;
        for (size_t x = 0; x < numberOfCellsInX; ++x) {
            for (size_t y = 0; y < numberOfCellsInY; ++y) {
                if (minVelocity > minVelocities[x][y])
                {
                    minVelocity = minVelocities[x][y];
                }
            }
        }
        return minVelocity;
    }
} // domain
} // syntheticSeismic
