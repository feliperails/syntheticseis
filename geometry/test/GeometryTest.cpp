#include <gtest/gtest.h>

#include <geometry/src/Coordinate.h>

namespace{
const double LIMIT = -std::numeric_limits<double>::max();
}

TEST(GeometryTest, CoordinateTest)
{
    using namespace  invertseis::geometry;

    Coordinate coordinate;

    ASSERT_EQ(coordinate.x(), LIMIT);
    ASSERT_EQ(coordinate.y(), LIMIT);
    ASSERT_EQ(coordinate.z(), LIMIT);

    coordinate = Coordinate(2.0, 3.0, 5.0);
    ASSERT_EQ(coordinate.x(), 2.0);
    ASSERT_EQ(coordinate.y(), 3.0);
    ASSERT_EQ(coordinate.z(), 5.0);
}
