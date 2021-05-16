#include "RotateVolumeCoordinate.h"
#include "geometry/src/Volume.h"
#include <CGAL/Cartesian.h>
#include <CGAL/Aff_transformation_3.h>
#include <math.h>
#include <omp.h>

using namespace syntheticSeismic::geometry;

typedef CGAL::Cartesian<double> CgalKernel;
typedef CGAL::Aff_transformation_3<CgalKernel> CgalTransformation;
typedef CGAL::Point_3<CgalKernel> CgalPoint3D;
typedef CGAL::Vector_3<CgalKernel> CgalVector3D;

#define ROTATE_POINTS_OPENMP_CHUNK 1000

namespace syntheticSeismic {
namespace domain {

std::pair<Point2D, double> RotateVolumeCoordinate::calculateReferencePoint(const std::array<Point2D, 4> &minimumRectanglePoints)
{
    double smallestX = minimumRectanglePoints[0].x;
    double smallestYAmongSmallestX = minimumRectanglePoints[0].y;
    size_t indexReferencePoint = 0;

    // Finds the points with the smallest x.
    for (size_t i = 1; i < minimumRectanglePoints.size(); ++i)
    {
        if (minimumRectanglePoints[i].x < smallestX ||
                (qFuzzyCompare(minimumRectanglePoints[i].x, smallestX) && minimumRectanglePoints[i].y < smallestYAmongSmallestX))
        {
            smallestX = minimumRectanglePoints[i].x;
            smallestYAmongSmallestX = minimumRectanglePoints[i].y;
            indexReferencePoint = i;
        }
    }

    // Finds the adjacent point with the y less than or equal to the reference point.
    size_t indexAdjacentPoint = 0;
    const std::vector<size_t> adjacentIndexes = {(indexReferencePoint + 1) % 4, (indexReferencePoint + 3) % 4};
    for (const size_t i : adjacentIndexes)
    {
        if (minimumRectanglePoints[i].y < smallestYAmongSmallestX ||
                qFuzzyCompare(minimumRectanglePoints[i].y, smallestYAmongSmallestX))
        {
            indexAdjacentPoint = i;
            break;
        }
    }

    double angle = 0.0;
    if (!qFuzzyCompare(minimumRectanglePoints[indexAdjacentPoint].y, minimumRectanglePoints[indexReferencePoint].y))
    {
        const double oppositeSide = minimumRectanglePoints[indexReferencePoint].y - minimumRectanglePoints[indexAdjacentPoint].y;
        const double adjacentSide = minimumRectanglePoints[indexReferencePoint].x - minimumRectanglePoints[indexAdjacentPoint].x;
        const double hypotenuse = std::sqrt(adjacentSide * adjacentSide + oppositeSide * oppositeSide);

        angle = std::asin(oppositeSide / hypotenuse);
    }

    return {minimumRectanglePoints[indexReferencePoint], angle};
}

std::pair<double, double> RotateVolumeCoordinate::calculateMinimumAndMaximumZ(const std::vector<std::shared_ptr<Volume>> &volumes)
{
    double minimumZ = volumes[0]->points[0].z;
    double maximumZ = volumes[0]->points[0].z;
    for (auto volume : volumes)
    {
        for (auto point : volume->points)
        {
            if (point.z < minimumZ)
            {
                minimumZ = point.z;
            }
            if (point.z > maximumZ)
            {
                maximumZ = point.z;
            }
        }
    }

    return {minimumZ, maximumZ};
}

std::shared_ptr<RotateVolumeCoordinate::Result> RotateVolumeCoordinate::rotateByMinimumRectangle(std::vector<std::shared_ptr<Volume>> &volumes, const std::array<Point2D, 4> &minimumRectanglePoints)
{
    const auto minimumAndMaximumZ = calculateMinimumAndMaximumZ(volumes);

    const auto referencePointAndAngleInRadians = calculateReferencePoint(minimumRectanglePoints);
    const geometry::Point3D referencePoint(
                referencePointAndAngleInRadians.first.x,
                referencePointAndAngleInRadians.first.y,
                minimumAndMaximumZ.first
            );

    if (qFuzzyCompare(referencePointAndAngleInRadians.second, 0.0))
    {
        for (auto &volume : volumes)
        {
            for (auto &point : volume->points)
            {
                point.x -= referencePoint.x;
                point.y -= referencePoint.y;
                point.z -= referencePoint.z;
            }
        }
    }
    else
    {
        const auto angle = referencePointAndAngleInRadians.second;

        const CgalTransformation transformation(cos(angle),   -sin(angle),  0.0,
                                                sin(angle),    cos(angle),  0.0,
                                                0.0,           0.0,         1.0);

        const int volumesSize = static_cast<int>(volumes.size());

        #pragma omp parallel for schedule(dynamic, ROTATE_POINTS_OPENMP_CHUNK)
        for (int i = 0; i < volumesSize; ++i)
        {
            for (auto &point : volumes[static_cast<size_t>(i)]->points)
            {
                const CgalPoint3D pointCgal(point.x - referencePoint.x, point.y - referencePoint.y, point.z - referencePoint.z);
                const auto pointCgalRotated = transformation(pointCgal);
                point.x = pointCgalRotated.x();
                point.y = pointCgalRotated.y();
                point.z = pointCgalRotated.z();
            }
        }
    }

    return std::make_shared<Result>(
                referencePoint,
                minimumAndMaximumZ.first,
                minimumAndMaximumZ.second,
                referencePointAndAngleInRadians.second
            );
}



} // namespace domain
} // namespace syntheticSeismic
