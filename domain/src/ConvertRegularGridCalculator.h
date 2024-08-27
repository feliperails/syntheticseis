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
    bool m_defineMissingLithologyByProximity = true;

    explicit ConvertRegularGridCalculator(std::shared_ptr<Lithology> undefinedLithology);

    void addLithology(const std::shared_ptr<Lithology>& lithology);

    RegularGrid<std::shared_ptr<geometry::Volume>> fromZInMetersToZInSeconds(RegularGrid<std::shared_ptr<geometry::Volume>> &depthGrid);

    RegularGrid<double> fromZInSecondsToZInMeters(RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology,
                                                                             RegularGrid<double> &timeGridTrace);

    RegularGrid<std::shared_ptr<geometry::Volume>> fillLithologyKeepUndefinedTimeGrid(
        RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology);

    RegularGrid<std::shared_ptr<geometry::Volume>> fillSingleLithologyTimeGrid(
        RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology, const std::shared_ptr<Lithology>& lithologyToFill);

    RegularGrid<std::shared_ptr<geometry::Volume>> fillTopBottomLithologyTimeGrid(
        RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology,
        const std::shared_ptr<Lithology>& topLithology,
        const std::shared_ptr<Lithology>& bottomLithology);

    RegularGrid<std::shared_ptr<geometry::Volume>>
                        fillLithologyTimeGrid(RegularGrid<std::shared_ptr<geometry::Volume>> &timeGridLithology,
                        const std::shared_ptr<Lithology>& lithologyToFill);


private:
    std::map<int, std::shared_ptr<Lithology>> m_lithologies;
    std::shared_ptr<Lithology> m_undefinedLithology;

    std::pair<double, double> computeMaxVelocityAndElapsedTime(RegularGrid<std::shared_ptr<geometry::Volume>> &metersGrid);

    double computeMinVelocity(RegularGrid <std::shared_ptr<geometry::Volume>> &metersGrid);

    std::pair<double, bool> getVelocity(
        RegularGrid<std::shared_ptr<geometry::Volume>> &depthGrid,
        size_t x,
        size_t y,
        size_t z
    );

    std::pair<double, bool> getNearestVelocity(
        std::vector<std::vector<std::vector<std::shared_ptr<geometry::Volume>>>> &data,
        int x, int y, int z,
        int limitX, int limitY, int limitZ
    );
};

} // domain
} // syntheticSeismic
