#pragma once
#include <vector>
#include "ExtractVolumes.h"
#include "Point2D.h"

namespace syntheticSeismic {
namespace domain {

class extractMinimumRectangle2D
{
public:
    static std::vector<Point2D> extractFrom(const std::vector<Volume>& volumes);
private:
    // Class with static methods only. It must not be instantiated.
    extractMinimumRectangle2D() = default;
};

} // namespace domain
} // namespace syntheticSeismic
