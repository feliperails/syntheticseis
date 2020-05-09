#include "Coordinate.h"

#include <limits>
#include <QtGlobal>

namespace syntheticSeismic {
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
    return qFuzzyCompare(other.m_x, m_x)
            && qFuzzyCompare(other.m_y, m_y)
            && qFuzzyCompare(other.m_z, m_z);
}

}
}
