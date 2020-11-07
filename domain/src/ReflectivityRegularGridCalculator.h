#pragma once
#include <map>
#include <memory>
#include "RegularGrid.h"

namespace syntheticSeismic {

namespace geometry {
class Volume;
}

namespace domain {
class ReflectivityRegularGridCalculator
{
public:
    ReflectivityRegularGridCalculator(double undefinedImpedance);
    std::shared_ptr<RegularGrid<double>> execute(RegularGrid<double> &regularVolumeGrid);
private:
    double m_undefinedImpedance;
};

} // namespace domain
} // namespace syntheticSeismic
