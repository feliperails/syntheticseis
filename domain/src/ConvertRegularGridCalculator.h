#pragma once

#include "Lithology.h"
#include "RegularGrid.h"
#include "geometry/src/Volume.h"
#include <map>
#include <memory>

namespace syntheticSeismic {
namespace domain {

class ConvertRegularGridCalculator {
public:
    ConvertRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology);

    void addLithology(const std::shared_ptr<Lithology>& lithology);

    RegularGrid<std::shared_ptr<geometry::Volume>> fromZInMetersToZInSeconds(RegularGrid<std::shared_ptr<geometry::Volume>> &depthGrid);

    std::shared_ptr<RegularGrid<std::shared_ptr<geometry::Volume>>> fromZInSecondsToZInMeters(RegularGrid<std::shared_ptr<geometry::Volume>> &timeGrid);
private:
    std::map<int, std::shared_ptr<Lithology>> m_lithologies;
    std::shared_ptr<Lithology> m_undefinedLithology;

    std::pair<double, double> computeMaxVelocityAndElapsedTime(RegularGrid<std::shared_ptr<geometry::Volume>> &metersGrid);
};

} // domain
} // syntheticSeismic
