#pragma once

#include "CellSizeCalculator.h"

namespace syntheticSeismic {
namespace domain {

CellSizeCalculator::CellSizeCalculator(const size_t& numberOfCellsInX, const size_t& numberOfCellsInY, const size_t& numberOfCellsInZ, const double& rickerWaveletFrequency, const std::vector<std::shared_ptr<geometry::Volume>>& volumes) :
    m_numberOfCellsInX(numberOfCellsInX),
    m_numberOfCellsInY(numberOfCellsInY),
    m_numberOfCellsInZ(numberOfCellsInZ),
    m_rickerWaveletFrequency(rickerWaveletFrequency),
    m_volumes(volumes)
{
    calculate();
}

const double& CellSizeCalculator::getCellSizeInX() const
{
    return m_cellSizeInX;
}

const double& CellSizeCalculator::getCellSizeInY() const
{
    return m_cellSizeInY;
}

const double& CellSizeCalculator::getCellSizeInZ() const
{
    return m_cellSizeInZ;
}

void CellSizeCalculator::calculate()
{
    constexpr double maxDouble = std::numeric_limits<double>::max();
    double maxX = -maxDouble;
    double maxY = -maxDouble;
    double maxZ = -maxDouble;

    for (size_t i = 0; i < m_volumes.size(); ++i)
    {
        for (size_t j = 0; j < m_volumes[i]->points.size(); ++j)
        {
            const auto &point = m_volumes[i]->points[j];
            if (point.x > maxX)
            {
                maxX = point.x;
            }

            if (point.y > maxY)
            {
                maxY = point.y;
            }

            if (point.z > maxZ)
            {
                maxZ = point.z;
            }
        }
    }

    m_cellSizeInX = maxX / m_numberOfCellsInX;
    m_cellSizeInY = maxY / m_numberOfCellsInY;
    m_cellSizeInZ = maxZ / m_numberOfCellsInZ;
}

} // namespace domain
} // namespace syntheticSeismic
