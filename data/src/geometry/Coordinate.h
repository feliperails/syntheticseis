#pragma once

namespace invertseis {
namespace geometry {

class Coordinate
{
public:
    Coordinate();
    Coordinate(const double x, const double y, const double z);

    Coordinate(const Coordinate&) = default;

    Coordinate& operator =(const Coordinate&) = default;

    bool operator ==(const Coordinate&) const;

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
