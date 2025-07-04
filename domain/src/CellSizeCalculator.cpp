#pragma once

#include "CellSizeCalculator.h"
#include <cmath>

namespace syntheticSeismic {
namespace domain {

CellSizeCalculator::CellSizeCalculator(const size_t& numberOfCellsInX, const size_t& numberOfCellsInY, const size_t& numberOfCellsInZ, const double& rickerWaveletFrequency, const std::array<geometry::Point2D, 4>& minimumRectangle) :
    m_numberOfCellsInX(numberOfCellsInX),
    m_numberOfCellsInY(numberOfCellsInY),
    m_numberOfCellsInZ(numberOfCellsInZ),
    m_rickerWaveletFrequency(rickerWaveletFrequency),
    m_minimumRectangle(minimumRectangle)
{
    calculateDeltas();
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
    m_cellSizeInZ = 0.0;
    m_cellSizeInX = m_dX / m_numberOfCellsInX;
    m_cellSizeInY = m_dY / m_numberOfCellsInY;

    if (m_rickerWaveletFrequency > 0.0)
    {
        m_cellSizeInZ = m_averageVelocity / (4.0 * m_rickerWaveletFrequency);
    }
}

void CellSizeCalculator::calculateDeltas()
{
    m_dX = std::hypot(m_minimumRectangle[1].x - m_minimumRectangle[0].x, m_minimumRectangle[1].y - m_minimumRectangle[0].y);
    m_dY = std::hypot(m_minimumRectangle[2].x - m_minimumRectangle[1].x, m_minimumRectangle[2].y - m_minimumRectangle[1].y);
}

} // namespace domain
} // namespace syntheticSeismic
