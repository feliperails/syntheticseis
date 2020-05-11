#ifndef POINT2D_H
#define POINT2D_H


namespace syntheticSeismic {
namespace domain {

class Point2D
{
public:
    double x = 0.0;
    double y = 0.0;

    Point2D() = default;
    Point2D(double x, double y): x(x), y(y)
    {

    }
};

} // namespace domain
} // namespace syntheticSeismic

#endif // POINT2D_H
