#pragma once

#include "data/src/geometry/Coordinate.h"
#include "data/src/DomainObject.h"

#include <QVector>

#include "RegularGrid.h"
#include "EclipseGrid.h"

#include <array>

namespace invertseis {
namespace domain {

class Point3D
{
public:
    double x;
    double y;
    double z;
};

class Volume
{
public:
    std::array<Point3D, 8> m_points;
};

class ExtractVolumes
{
public:
    static std::vector<Volume> extractFirstLayerFrom(const invertseis::domain::EclipseGrid& eclipseGrid);
};

} // namespace domain
} // namespace invertseis
