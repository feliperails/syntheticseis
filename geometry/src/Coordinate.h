#ifndef INC_INVERTSEIS_GEOMETRY_H
#define INC_INVERTSEIS_GEOMETRY_H

namespace invertseis {
namespace geometry {

class Coordinate
{
public:
    Coordinate(const double x, const double y, const double z);

    double x() const{ return m_x; }
    double y() const{ return m_y; }
    double z() const{ return m_z; }

private:
    double m_x;
    double m_y;
    double m_z;
};

} // namespace geometry
} // namespace invertseis
#endif
