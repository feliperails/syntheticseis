#pragma once

#include "Lithology.h"
#include "RegularGrid.h"
#include "geometry/src/Volume.h"
#include <map>

namespace syntheticSeismic {
namespace domain {

class ConvertRegularGridCalculator {
public:
    ConvertRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology);

    void addLithology(std::shared_ptr<Lithology> lithology);

    std::shared_ptr<RegularGrid<double>> fromZInMetersToZInTime(RegularGrid<std::shared_ptr<geometry::Volume>> &grid);
private:
    std::map<int, std::shared_ptr<Lithology>> m_lithologies;
    std::shared_ptr<Lithology> m_undefinedLithology;

    double computeMaxVelocity(RegularGrid <std::shared_ptr<geometry::Volume>> &grid);
};

} // domain
} // syntheticSeismic
