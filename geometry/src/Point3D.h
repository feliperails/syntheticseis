#pragma once

namespace syntheticSeismic {
namespace geometry {

class Point3D
{
public:
    double x;
    double y;
    double z;

    Point3D() : x(0.0), y(0.0), z(0.0)
    {

    }
    Point3D(double x, double y, double z): x(x), y(y), z(z)
    {

    }
};

} // namespace geometry
} // namespace syntheticSeismic
