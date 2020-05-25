#pragma once

#include "ExtractVolumes.h"
#include "Point2D.h"

namespace syntheticSeismic {
namespace domain {

class RotateVolumeCoordinate
{
public:
    static void rotateByMinimumRectangle(std::vector<Volume>& volumes, const std::vector<Point2D>& minimumRectanglePoints);

    /// Calculates the reference point consisting of the point with the smallest x in the rectngle and the angle formed with its adjacent point,
    /// whose y is equal to or less than the reference point.
    /// @param Rectangle points
    /// @return Reference point and angle with the adjacent point in radians.
    static std::pair<Point2D, double> calculateReferencePoint(const std::vector<Point2D> &minimumRectanglePoints);
private:
    // Class with static methods only. It must not be instantiated.
    RotateVolumeCoordinate() = default;

    /// Calculates the minimum and maximum value in z
    /// @param Volumes
    /// @return Minimum z in the first position of the std::pair and maximum z in the second position of the std::pair.
    static std::pair<double, double> calculateMinimumAndMaximumZ(const std::vector<Volume> &volumes);
};

} // namespace domain
} // namespace syntheticSeismic
