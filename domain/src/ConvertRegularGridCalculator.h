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
    explicit ConvertRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology);

    void addLithology(const std::shared_ptr<Lithology>& lithology);

    RegularGrid<std::shared_ptr<geometry::Volume>> fromZInMetersToZInSeconds(RegularGrid<std::shared_ptr<geometry::Volume>> &depthGrid);

    RegularGrid<double> fromZInSecondsToZInMeters(RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology,
                                                                             RegularGrid<double> &timeGridTrace);
private:
    std::map<int, std::shared_ptr<Lithology>> m_lithologies;
    std::shared_ptr<Lithology> m_undefinedLithology;

    std::pair<double, double> computeMaxVelocityAndElapsedTime(RegularGrid<std::shared_ptr<geometry::Volume>> &metersGrid);

    double computeMinVelocity(RegularGrid <std::shared_ptr<geometry::Volume>> &metersGrid);
};

} // domain
} // syntheticSeismic
