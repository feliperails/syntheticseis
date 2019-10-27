#include "Coordinate.h"

#include <limits>

namespace invertseis {
namespace geometry {

namespace{
const double LIMIT = -std::numeric_limits<double>::max();
}

Coordinate::Coordinate()
    : m_x(LIMIT)
    , m_y(LIMIT)
    , m_z(LIMIT)
{
}

Coordinate::Coordinate(const double x, const double y, const double z)
    : m_x(x)
    , m_y(y)
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
