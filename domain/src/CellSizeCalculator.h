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
    CellSizeCalculator(const size_t& numberOfCellsInX, const size_t& numberOfCellsInY, const size_t& numberOfCellsInZ, const double& rickerWaveletFrequency, const std::vector<std::shared_ptr<geometry::Volume>>& volumes);

    const double& getCellSizeInX() const;
    const double& getCellSizeInY() const;
    const double& getCellSizeInZ() const;

private:
    void calculate();

private:
    double m_cellSizeInX;
    double m_cellSizeInY;
    double m_cellSizeInZ;

    const size_t& m_numberOfCellsInX;
    const size_t& m_numberOfCellsInY;
    const size_t& m_numberOfCellsInZ;
    const double& m_rickerWaveletFrequency;
    const std::vector<std::shared_ptr<geometry::Volume>>& m_volumes;
};

} // namespace domain
} // namespace syntheticSeismic
