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

    inline static size_t calculateReorderedIndex(const size_t originalIndex)
    {
        // Reorders the points on the volume to store all points on the first layer first and then all points on the second layer.
        // The points are organized in this way to facilitate the drawing of the volume.
        // Mapping
        // original   reordered
        //    0           0
        //    1           2
        //    2           4
        //    3           6
        //    4           1
        //    5           3
        //    6           5
        //    7           7
        return (originalIndex - originalIndex % 2) / 2 + originalIndex % 2 * 4;
    }
};

} // namespace domain
} // namespace syntheticSeismic
