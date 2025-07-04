#pragma once

#include "CellSizeCalculator.h"

#include <iostream>

namespace syntheticSeismic {
namespace domain {

CellSizeCalculator::CellSizeCalculator(const size_t& numberOfCellsInX, const size_t& numberOfCellsInY, const size_t& numberOfCellsInZ, const double& rickerWaveletFrequency, const std::vector<std::shared_ptr<geometry::Volume>>& volumes, const double& averageVelocity /*= 2000.0*/) :
    m_numberOfCellsInX(numberOfCellsInX),
    m_numberOfCellsInY(numberOfCellsInY),
    m_numberOfCellsInZ(numberOfCellsInZ),
    m_rickerWaveletFrequency(rickerWaveletFrequency),
    m_volumes(volumes),
    m_averageVelocity(averageVelocity)
{
    calculateMinMax();
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

void CellSizeCalculator::setNumberOfCellsInX(const size_t& numberOfCellsInX)
{
    m_numberOfCellsInX = numberOfCellsInX;
}

void CellSizeCalculator::setNumberOfCellsInY(const size_t& numberOfCellsInY)
{
    m_numberOfCellsInY = numberOfCellsInY;
}

void CellSizeCalculator::setNumberOfCellsInZ(const size_t& numberOfCellsInZ)
{
    m_numberOfCellsInZ = numberOfCellsInZ;
}

void CellSizeCalculator::setRickerWaveletFrequency(const double& rickerWaveletFrequency)
{
    m_rickerWaveletFrequency = rickerWaveletFrequency;
}

void CellSizeCalculator::calculate()
{
    m_cellSizeInX = (m_maxX - m_minX) / m_numberOfCellsInX;

    m_cellSizeInY = (m_maxY - m_minY) / m_numberOfCellsInY;

    std::cout << "m_rickerWaveletFrequency: " << m_rickerWaveletFrequency << std::endl;
    std::cout << "m_averageVelocity: " << m_averageVelocity << std::endl;
    if (m_rickerWaveletFrequency > 0.0)
    {
        std::cout << "m_rickerWaveletFrequency > 0.0" << m_averageVelocity << std::endl;

        m_cellSizeInZ = m_averageVelocity / (4.0 * m_rickerWaveletFrequency);

        std::cout << "m_cellSizeInZ: " << m_cellSizeInZ << std::endl;
    }
    else
    {
        m_cellSizeInZ = (m_maxZ - m_minZ) / m_numberOfCellsInZ;
    }
}

void CellSizeCalculator::calculateMinMax()
{
    constexpr double maxDouble = std::numeric_limits<double>::max();

    m_minX = maxDouble;
    m_minY = maxDouble;
    m_minZ = maxDouble;

    m_maxX = -maxDouble;
    m_maxY = -maxDouble;
    m_maxZ = -maxDouble;

    for (size_t i = 0; i < m_volumes.size(); ++i)
    {
        for (size_t j = 0; j < m_volumes[i]->points.size(); ++j)
        {
            const auto &point = m_volumes[i]->points[j];

            if (point.x < m_minX)
            {
                m_minX = point.x;
            }

            if (point.y < m_minY)
            {
                m_minY = point.y;
            }

            if (point.z < m_minZ)
            {
                m_minZ = point.z;
            }

            if (point.x > m_maxX)
            {
                m_maxX = point.x;
            }

            if (point.y > m_maxY)
            {
                m_maxY = point.y;
            }

            if (point.z > m_maxZ)
            {
                m_maxZ = point.z;
            }
        }
    }

    if (qFuzzyCompare(m_minX, m_maxX))
    {
        m_minX = 0.0;
    }

    if (qFuzzyCompare(m_minY, m_maxY))
    {
        m_minY = 0.0;
    }

    if (qFuzzyCompare(m_minZ, m_maxZ))
    {
        m_minZ = 0.0;
    }
}

} // namespace domain
} // namespace syntheticSeismic
