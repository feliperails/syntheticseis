#include "Coordinate.h"

namespace invertseis {
namespace geometry {

Coordinate::Coordinate(const double x, const double y, const double z)
    : m_x(x)
    , m_y(z)
    , m_z(z)
{
}

bool Coordinate::operator==(const Coordinate& other) const
{
    return other.m_x == m_x
            && other.m_y == m_y
            && other.m_z == m_z;
}

}
}
