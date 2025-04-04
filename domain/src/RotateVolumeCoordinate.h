#pragma once

#include "ExtractVolumes.h"
#include "geometry/src/Point2D.h"
#include "geometry/src/Volume.h"

namespace syntheticSeismic {
namespace domain {

class RotateVolumeCoordinate
{
public:
    class Result
    {
        public:
            explicit Result(const geometry::Point3D &origin, const double minimumZ, const double maximumZ, const double angle)
                : origin(origin)
                , minimumZ(minimumZ)
                , maximumZ(maximumZ)
                , angle(angle)
            {

            }
            const geometry::Point3D origin;
            const double minimumZ;
            const double maximumZ;
            const double angle;
    };

    static std::shared_ptr<Result> rotateByMinimumRectangle(
            std::vector<std::shared_ptr<geometry::Volume>>& volumes,
            const std::array<geometry::Point2D, 4>& minimumRectanglePoints
        );

    /// Calculates the reference point consisting of the point with the smallest x in the rectngle and the angle formed with its adjacent point,
    /// whose y is equal to or less than the reference point.
    /// @param Rectangle points
    /// @return Reference point and angle with the adjacent point in radians.
    static std::pair<geometry::Point2D, double> calculateReferencePoint(const std::array<geometry::Point2D, 4> &minimumRectanglePoints);
private:
    // Class with static methods only. It must not be instantiated.
    explicit RotateVolumeCoordinate() = default;

    /// Calculates the minimum and maximum value in z
    /// @param Volumes
    /// @return Minimum z in the first position of the std::pair and maximum z in the second position of the std::pair.
    static std::pair<double, double> calculateMinimumAndMaximumZ(const std::vector<std::shared_ptr<geometry::Volume>> &volumes);
};

} // namespace domain
} // namespace syntheticSeismic
