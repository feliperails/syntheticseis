#include <iostream>
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
            throw std::exception("The Z grid unit must be in meters. Code: 0001");
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

                    const auto velocityResult = getVelocity(depthGrid, x, y, z);

                    currentTime += cellSizeInZ / velocityResult.first;

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

    RegularGrid<double> ConvertRegularGridCalculator::fromZInSecondsToZInMeters(
        RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology,
        RegularGrid<double> &timeGridTrace)
    {
        const size_t numberOfCellsInX = timeGridLithology.getNumberOfCellsInX();
        const size_t numberOfCellsInY = timeGridLithology.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = timeGridLithology.getNumberOfCellsInZ();
        const auto &timeGridLithologyData = timeGridLithology.getData();
        const auto &timeGridTraceData = timeGridTrace.getData();

        const auto timeStep = timeGridLithology.getCellSizeInZ();
        double minVelocity = computeMinVelocity(timeGridLithology);
        const double positionStep = timeStep * minVelocity;

        std::cout << "Time step  " << timeStep << std::endl;

        RegularGrid<double> depthGrid(
            numberOfCellsInX,
            numberOfCellsInY,
            0,
            timeGridLithology.getCellSizeInX(),
            timeGridLithology.getCellSizeInY(),
            positionStep,
            timeGridLithology.getUnitInX(),
            timeGridLithology.getUnitInY(),
            EnumUnit::Meters,
            timeGridLithology.getRectanglePoints(),
            timeGridLithology.getZBottom(),
            timeGridLithology.getZTop(),
            0.0
            );

        auto &depthData = depthGrid.getData();
        size_t maxNumPositionSteps = 0;
        // #pragma omp parallel for
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
                    if (timeGridLithologyData[x][y][z] != nullptr)
                    {
                        const auto idLithology = timeGridLithologyData[x][y][z]->idLithology;
                        const auto &lithology = *m_lithologies[idLithology];
                        velocity = lithology.getVelocity();
                    }
                    currentPosition += timeStep * velocity;
                    positionFromTime.push_back(currentPosition);
                }

                // Making the seismic trace equally splited in space to store in structure
                const double finalPosition = currentPosition;
                const size_t numPositionSteps = static_cast<size_t>(finalPosition/positionStep);
                if(numPositionSteps > maxNumPositionSteps)
                {
                    maxNumPositionSteps = numPositionSteps;
                }

                double seismic;
                double position = 0.0;

                depthData[x][y].resize(numPositionSteps);

                for(size_t depthIdxInt = 0; depthIdxInt < numPositionSteps-2; depthIdxInt++)
                {
                    position = depthIdxInt * positionStep;
                    size_t depthIdxBeforePositionInt = 0;
                    for(size_t z = 1; z < numberOfCellsInZ-1; z++)
                    {
                        if(positionFromTime[z] < position)
                        {
                            depthIdxBeforePositionInt = z;
                        }

                    }
                    seismic = (timeGridTraceData[x][y][depthIdxBeforePositionInt] * (position - positionFromTime[depthIdxBeforePositionInt]) /
                                   (positionFromTime[depthIdxBeforePositionInt+1] - positionFromTime[depthIdxBeforePositionInt])
                               + timeGridTraceData[x][y][depthIdxBeforePositionInt + 1] * (positionFromTime[depthIdxBeforePositionInt + 1] - position) /
                                     (positionFromTime[depthIdxBeforePositionInt+1] - positionFromTime[depthIdxBeforePositionInt]));
                    depthData[x][y][depthIdxInt] = seismic;
                }
            }
        }

        depthGrid.setNumberOfCellsInZ(maxNumPositionSteps);
        for (int xInt = 0; xInt < static_cast<int>(numberOfCellsInX); ++xInt)
        {
            const auto x = static_cast<size_t>(xInt);
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                if(depthData[x][y].size() < maxNumPositionSteps)
                {
                    for(size_t z = depthData[x][y].size(); z < maxNumPositionSteps; ++z)
                    {
                        depthData[x][y].push_back(0.0);
                    }
                }
            }
        }

        return depthGrid;
    }


    RegularGrid<std::shared_ptr<geometry::Volume>> ConvertRegularGridCalculator::fillLithologyTimeGrid(
        RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology, const std::shared_ptr<Lithology>& lithologyToFill)
    {
        size_t numberOfCellsInX = timeGridLithology.getNumberOfCellsInX();
        size_t numberOfCellsInY = timeGridLithology.getNumberOfCellsInY();
        size_t numberOfCellsInZ = timeGridLithology.getNumberOfCellsInZ();
        std::cout << "************************************" << std::endl;
        std::cout << numberOfCellsInX << std::endl;
        std::cout << numberOfCellsInY << std::endl;
        std::cout << numberOfCellsInZ << std::endl;
        std::cout << "************************************" << std::endl;

        auto timeGridData = timeGridLithology.getData();
        // if the user defined the lithology value to fill the gaps
        if (lithologyToFill->getId() != m_undefinedLithology->getId())
        {
            std::cout << "filling with fixed lithology" << std::endl;
            for (size_t x = 0; x < 1; ++x) //numberOfCellsInX; ++x)
            {
                for (size_t y = 0; y < 1; ++y) //numberOfCellsInY; ++y)
                {
                    for (size_t z = 0; z < numberOfCellsInZ; ++z)
                    {
                        if (timeGridData[x][y][z] != nullptr)
                        {
                            std::cout << timeGridData[x][y][z] << std::endl;
                            const auto idLithology = timeGridData[x][y][z]->idLithology;
                            const auto &lithology = *m_lithologies[idLithology];
                            if (lithology.getId() == m_undefinedLithology->getId())
                            {
                                std::cout << timeGridData[x][y][z]->idLithology << "  " << lithologyToFill->getId() << std::endl;
                                timeGridData[x][y][z]->idLithology = lithologyToFill->getId();
                            }
                        }
                        else
                        {

                            timeGridData[x][y][z]->idLithology = lithologyToFill->getId();
                        }
                    }
                }
            }
        }
        /*
        else
        {
            for (int xInt = 0; xInt < static_cast<int>(numberOfCellsInX); ++xInt)
            {
                const auto x = static_cast<size_t>(xInt);
                for (size_t y = 0; y < numberOfCellsInY; ++y)
                {
                    // filling bottom part with lithology
                    std::shared_ptr<Lithology> BottomLithology = nullptr;
                    auto timeVector = timeGridData[x][y];
                    for (size_t z = 0; z < numberOfCellsInZ; ++z)
                    {
                        const auto idLithology = timeVector[z]->idLithology;
                        const auto &lithology = *m_lithologies[idLithology];
                        if (BottomLithology->getId() != m_undefinedLithology->getId())
                        {
                            if (lithology.getId() == m_undefinedLithology->getId())
                            {
                                timeGridData[x][y][z]->idLithology = BottomLithology->getId();
                            }
                            else
                            {
                                BottomLithology = m_lithologies[timeGridData[x][y][z]->idLithology];
                            }
                        }
                        else
                        {
                            BottomLithology = m_lithologies[timeGridData[x][y][z]->idLithology];
                        }
                    }
                    // filling top part with lithology
                    std::shared_ptr<Lithology> TopLithology = nullptr;
                    for (size_t z = numberOfCellsInZ; z > 0; --z)
                    {
                        const auto idLithology = timeVector[z]->idLithology;
                        const auto &lithology = *m_lithologies[idLithology];
                        if (TopLithology->getId() != m_undefinedLithology->getId())
                        {
                            if (lithology.getId() == m_undefinedLithology->getId())
                            {
                                timeGridData[x][y][z]->idLithology = TopLithology->getId();
                            }
                            else
                            {
                                TopLithology = m_lithologies[timeGridData[x][y][z]->idLithology];
                            }
                        }
                        else
                        {
                            TopLithology = m_lithologies[timeGridData[x][y][z]->idLithology];
                        }
                    }
                }
            }
        }
        */
        return timeGridLithology;
    }

    std::pair<double, double> ConvertRegularGridCalculator::computeMaxVelocityAndElapsedTime(RegularGrid <std::shared_ptr<geometry::Volume>> &metersGrid) {
        const size_t numberOfCellsInX = metersGrid.getNumberOfCellsInX();
        const size_t numberOfCellsInY = metersGrid.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = metersGrid.getNumberOfCellsInZ();
        const int numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);
        const auto &metersData = metersGrid.getData();

        const auto cellSizeInZ = metersGrid.getCellSizeInZ();

        geometry::Volume *errorVolume = nullptr;
        size_t errorVolumeX, errorVolumeY, errorVolumeZ;

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
                    const auto velocityResult = getVelocity(metersGrid, x, y, z);

                    if (velocityResult.second)
                    {
                        errorVolume = metersData[x][y][z].get();
                        errorVolumeX = x;
                        errorVolumeY = y;
                        errorVolumeZ = z;
                        continue;
                    }

                    if (maxVelocities[x][y] < velocityResult.first)
                    {
                        maxVelocities[x][y] = velocityResult.first;
                    }

                    elapsedTimes[x][y] += cellSizeInZ / velocityResult.first;
                }
            }
        }
        if (errorVolume != nullptr)
        {
            const auto errorMessage = "Lithology " + QString::number(errorVolume->idLithology) + " of volume " +
                    QString::number(errorVolume->indexVolume) + " of " + QString::number(errorVolumeX) + ", " +
                    QString::number(errorVolumeY) + " and " + QString::number(errorVolumeZ) + " coordinates was not found. Code: 0002";
            throw std::exception(errorMessage.toStdString().c_str());
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

    double ConvertRegularGridCalculator::computeMinVelocity(RegularGrid <std::shared_ptr<geometry::Volume>> &timeGridLithology) {
        const size_t numberOfCellsInX = timeGridLithology.getNumberOfCellsInX();
        const size_t numberOfCellsInY = timeGridLithology.getNumberOfCellsInY();
        const size_t numberOfCellsInZ = timeGridLithology.getNumberOfCellsInZ();
        const int numberOfCellsInXInt = static_cast<int>(numberOfCellsInX);
        const auto &timeGridLithologyData = timeGridLithology.getData();

        geometry::Volume *errorVolume = nullptr;
        size_t errorVolumeX, errorVolumeY, errorVolumeZ;

        std::vector<std::vector<double>> minVelocities(numberOfCellsInX, std::vector<double>(numberOfCellsInY, 3000000.0));

        // #pragma omp parallel for
        for (int xInt = 0; xInt < numberOfCellsInXInt; ++xInt)
        {
            const auto x = static_cast<size_t>(xInt);
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                for (size_t z = 0; z < numberOfCellsInZ; ++z)
                {
                    const auto velocityResult = getVelocity(timeGridLithology, x, y, z);

                    if (velocityResult.second)
                    {
                        errorVolume = timeGridLithologyData[x][y][z].get();
                        errorVolumeX = x;
                        errorVolumeY = y;
                        errorVolumeZ = z;
                        break;
                    }
                    // std::cout << "x: " << x << "\t y: " << y << "\t z: " << z << " : " << velocityResult.second << std::endl;

                    if (minVelocities[x][y] > velocityResult.first)
                    {
                        minVelocities[x][y] = velocityResult.first;
                    }
                }
                if (errorVolume != nullptr) {
                    break;
                }
            }
            if (errorVolume != nullptr) {
                break;
            }
        }
        if (errorVolume != nullptr)
        {
            const auto errorMessage = "Lithology " + QString::number(errorVolume->idLithology) + " of volume " +
                                      QString::number(errorVolume->indexVolume) + " of " + QString::number(errorVolumeX) + ", " +
                                      QString::number(errorVolumeY) + " and " + QString::number(errorVolumeZ) + " coordinates was not found. Code: 0003";
            throw std::exception(errorMessage.toStdString().c_str());
        }

        auto minVelocity = std::numeric_limits<double>::max();
        for (size_t x = 0; x < numberOfCellsInX; ++x)
        {
            for (size_t y = 0; y < numberOfCellsInY; ++y)
            {
                if (minVelocity > minVelocities[x][y])
                {
                    minVelocity = minVelocities[x][y];
                }
            }
        }
        if (minVelocity < 1)
        {
            minVelocity = 1.0;
        }

        return minVelocity;
    }



    std::pair<double, bool> ConvertRegularGridCalculator::getVelocity(
        RegularGrid<std::shared_ptr<geometry::Volume>> &depthGrid,
        size_t x, size_t y, size_t z
    )
    {
        auto &data = depthGrid.getData();

        if (data[x][y][z] == nullptr)
        {
            return {m_undefinedLithology->getVelocity(), false};
        }

        const auto idLithology = data[x][y][z]->idLithology;

        if (m_lithologies.count(idLithology) == 0)
        {
            if (m_defineMissingLithologyByProximity)
            {
                const auto limitX = static_cast<int>(depthGrid.getNumberOfCellsInX() - 1);
                const auto limitY = static_cast<int>(depthGrid.getNumberOfCellsInY() - 1);
                const auto limitZ = static_cast<int>(depthGrid.getNumberOfCellsInZ() - 1);

                std::cout << "missing lithology: " << idLithology << " for (" << x << ", " << y << ", " << z << ")" << std::endl;

                return getNearestVelocity(
                        data, static_cast<int>(x), static_cast<int>(y), static_cast<int>(z),
                        limitX, limitY, limitZ
                    );
            }

            return {0.0, true};
        }


        return {m_lithologies[idLithology]->getVelocity(), false};
    }

    std::pair<double, bool> ConvertRegularGridCalculator::getNearestVelocity(
            std::vector<std::vector<std::vector<std::shared_ptr<geometry::Volume>>>> &data,
            int x, int y, int z, int limitX, int limitY, int limitZ
    )
    {
        for (int factor = 1; factor < 100; ++factor)
        {
            if (
                    (x - factor) < 0 && (x + factor) > limitX
                    && (y - factor) < 0 && (y + factor) > limitY
                    && (z - factor) < 0 && (z + factor) > limitZ
                    )
            {
                // return {0.0, true};
                return {m_undefinedLithology->getVelocity(), false};
            }

            const auto realLeftLimitX = x - factor;
            const auto realRightLimitX = x + factor;
            const auto realLeftLimitY = y - factor;
            const auto realRightLimitY = y + factor;
            const auto realLeftLimitZ = z - factor;
            const auto realRightLimitZ = z + factor;

            const auto leftLimitX = std::max(realLeftLimitX, 0);
            const auto rightLimitX = std::min(realRightLimitX, limitX);
            const auto leftLimitY = std::max(realLeftLimitY, 0);
            const auto rightLimitY = std::min(realRightLimitY, limitY);
            const auto leftLimitZ = std::max(realLeftLimitZ, 0);
            const auto rightLimitZ = std::min(realRightLimitZ, limitZ);
            for (int newX = leftLimitX; newX <= rightLimitX; ++newX) {
                for (int newY = leftLimitY; newY <= rightLimitY; ++newY) {
                    for (int newZ = leftLimitZ; newZ <= rightLimitZ; ++newZ) {
                        if (
                                newX == realLeftLimitX || newX == realRightLimitX
                                || newY == realLeftLimitY || newY == realRightLimitY
                                || newZ == realLeftLimitZ || newZ == realRightLimitZ
                                ) {
                            if (data[newX][newY][newZ] == nullptr)
                            {
                                continue;
                            }

                            const auto newIdLithology = data[newX][newY][newZ]->idLithology;

                            if (m_lithologies.count(newIdLithology) == 0) {
                                continue;
                            }

                            const auto &newLithology = *m_lithologies[newIdLithology];
                            data[x][y][z]->idLithology = newIdLithology;

                            return {newLithology.getVelocity(), false};
                        }
                    }
                }
            }
        }

        // return {0.0, true};
        return {m_undefinedLithology->getVelocity(), false};
    }
} // domain
} // syntheticSeismic
