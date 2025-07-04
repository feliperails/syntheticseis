#pragma once
#include <array>

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

using MinRectangle2D = std::array<Point2D, 4>;

} // namespace domain
} // namespace syntheticSeismic
