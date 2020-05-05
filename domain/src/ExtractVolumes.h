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
                                                                  const syntheticSeismic::domain::EclipseGrid& eclipseGrid);
};

} // namespace domain
} // namespace syntheticSeismic
