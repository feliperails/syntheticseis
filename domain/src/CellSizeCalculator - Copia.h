#pragma once

#include <vector>
#include <array>
#include "ExtractVolumes.h"
#include "geometry/src/Point2D.h"

namespace syntheticSeismic {
namespace domain {

class CellSizeCalculator
{
public:
    CellSizeCalculator(const size_t& numberOfCellsInX, const size_t& numberOfCellsInY, const size_t& numberOfCellsInZ, const double& rickerWaveletFrequency, const std::vector<std::shared_ptr<geometry::Volume>>& volumes, const double& averageVelocity = 2500.0);

    void calculate();

    const double& getCellSizeInX() const;
    const double& getCellSizeInY() const;
    const double& getCellSizeInZ() const;

    void setNumberOfCellsInX(const size_t& numberOfCellsInX);
    void setNumberOfCellsInY(const size_t& numberOfCellsInY);
    void setNumberOfCellsInZ(const size_t& numberOfCellsInZ);
    void setRickerWaveletFrequency(const double& rickerWaveletFrequency);

private:
    void calculateMinMax();

private:
    double m_cellSizeInX;
    double m_cellSizeInY;
    double m_cellSizeInZ;

    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
    double m_rickerWaveletFrequency;
    const std::vector<std::shared_ptr<geometry::Volume>>& m_volumes;
    const double m_averageVelocity = 2500.0;

    double m_minX;
    double m_minY;
    double m_minZ;

    double m_maxX;
    double m_maxY;
    double m_maxZ;
};

} // namespace domain
} // namespace syntheticSeismic
