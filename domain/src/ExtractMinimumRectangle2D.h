#pragma once

#include <vector>
#include <array>
#include "ExtractVolumes.h"
#include "geometry/src/Point2D.h"

namespace syntheticSeismic {
namespace domain {

class ExtractMinimumRectangle2D
{
public:
    static std::array<geometry::Point2D, 4> extractFrom(const std::vector<std::shared_ptr<geometry::Volume>>& volumes);
private:
    // Class with static methods only. It must not be instantiated.
    ExtractMinimumRectangle2D() = default;
};

} // namespace domain
} // namespace syntheticSeismic
