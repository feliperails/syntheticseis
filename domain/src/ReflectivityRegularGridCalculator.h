#pragma once
#include <map>
#include <memory>
#include "RegularGrid.h"

namespace syntheticSeismic {

namespace geometry {
class Volume;
}

namespace domain {

class Lithology;

class ReflectivityRegularGridCalculator
{
public:
    ReflectivityRegularGridCalculator(double m_undefinedImpedance);
    RegularGrid<double> convert(const RegularGrid<double> &regularVolumeGrid);
private:
    double m_undefinedImpedance;
};

} // namespace domain
} // namespace syntheticSeismic
