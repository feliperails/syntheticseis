#pragma once

#include "geometry/src/Coordinate.h"
#include <QVector>
#include "RegularGrid.h"
#include "EclipseGrid.h"
#include <array>

namespace syntheticSeismic {
namespace domain {

class Point3D
{
public:
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Point3D() = default;
    Point3D(double x, double y, double z): x(x), y(y), z(z)
    {

    }
};

class Volume
{
public:
    std::array<Point3D, 8> m_points;
};

class ExtractVolumes
{
public:
    static std::vector<Volume> extractFirstLayerFrom(const syntheticSeismic::domain::EclipseGrid& eclipseGrid);

    static std::vector<Volume> extractFromVolumesOfFirstLayer(const std::vector<Volume>& volumesOfFirstLayer,
                                                              const syntheticSeismic::domain::EclipseGrid& eclipseGrid,
                                                              bool divideXandYIntoZ = false);

    inline static size_t calculateReorderedIndex(size_t originalIndex)
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
