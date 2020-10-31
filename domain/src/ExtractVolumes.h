#pragma once

#include "geometry/src/Coordinate.h"
#include "geometry/src/Point3D.h"
#include "geometry/src/Volume.h"
#include <QVector>
#include "RegularGrid.h"
#include "EclipseGrid.h"
#include <array>
#include <memory>

namespace syntheticSeismic {
namespace domain {

class ExtractVolumes
{
public:
    static std::vector<std::shared_ptr<geometry::Volume>> extractFirstLayerFrom(const syntheticSeismic::domain::EclipseGrid& eclipseGrid);

    static std::vector<std::shared_ptr<geometry::Volume>> extractFromVolumesOfFirstLayer(
            const std::vector<std::shared_ptr<geometry::Volume>>& volumesOfFirstLayer,
            const syntheticSeismic::domain::EclipseGrid& eclipseGrid,
            const bool divideXandYIntoZ = false
        );

    static size_t calculateReorderedIndex(const size_t originalIndex);
};

} // namespace domain
} // namespace syntheticSeismic
