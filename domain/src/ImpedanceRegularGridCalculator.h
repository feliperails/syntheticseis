#pragma once

#include "RegularGrid.h"
#include <memory>
#include <map>

namespace syntheticSeismic {

namespace geometry {
    class Volume;
}

namespace domain {
class Lithology;

class ImpedanceRegularGridCalculator
{
public:
    ImpedanceRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology);

    void addLithology(std::shared_ptr<Lithology> lithology);
    std::shared_ptr<RegularGrid<double>> convert(RegularGrid<std::shared_ptr<geometry::Volume>> regularVolumeGrid);
private:
    std::map<int, std::shared_ptr<Lithology>> m_lithologies;
    std::shared_ptr<Lithology> m_undefinedLithology;
};

} // namespace domain
} // namespace syntheticSeismic