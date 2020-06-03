#pragma once

namespace syntheticSeismic {
namespace geometry {

class Point2D
{
public:
    double x;
    double y;

    Point2D() : x(0.0), y (0.0)
    {

    }
    Point2D(double x, double y): x(x), y(y)
    {

    }
};

} // namespace domain
} // namespace syntheticSeismic
