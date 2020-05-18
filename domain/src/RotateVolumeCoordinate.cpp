#include "RotateVolumeCoordinate.h"
#include <iostream>
#include <CGAL/Cartesian.h>
#include <CGAL/Aff_transformation_3.h>
#include <math.h>
#include <omp.h>

typedef CGAL::Cartesian<double> CgalKernel;
typedef CGAL::Aff_transformation_3<CgalKernel> CgalTransformation;
typedef CGAL::Point_3<CgalKernel> CgalPoint3D;
typedef CGAL::Vector_3<CgalKernel> CgalVector3D;

#define ROTATE_POINTS_OPENMP_CHUNK 1000

namespace syntheticSeismic {
namespace domain {

std::pair<Point2D, double> RotateVolumeCoordinate::calculateReferencePoint(const std::vector<Point2D> &minimumRectanglePoints)
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
    std::vector<size_t> adjacentIndexes = {(indexReferencePoint + 1) % 4, (indexReferencePoint + 3) % 4};
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
        double oppositeSide = minimumRectanglePoints[indexReferencePoint].y - minimumRectanglePoints[indexAdjacentPoint].y;
        double adjacentSide = minimumRectanglePoints[indexReferencePoint].x - minimumRectanglePoints[indexAdjacentPoint].x;
        double hypotenuse = std::sqrt(adjacentSide * adjacentSide + oppositeSide * oppositeSide);

        angle = std::asin(oppositeSide / hypotenuse);
    }

    return {minimumRectanglePoints[indexReferencePoint], angle};
}

double RotateVolumeCoordinate::calculateMinimumZ(const std::vector<Volume> &volumes)
{
    double minimumZ = volumes[0].m_points[0].z;
    for (auto volume : volumes)
    {
        for (auto point : volume.m_points)
        {
            if (point.z < minimumZ)
            {
                minimumZ = point.z;
            }
        }
    }

    return minimumZ;
}

void RotateVolumeCoordinate::rotateByMinimumRectangle(std::vector<Volume> &volumes, const std::vector<Point2D> &minimumRectanglePoints)
{
    const double minimumZ = calculateMinimumZ(volumes);

    const auto referencePointAndAngleInRadians = calculateReferencePoint(minimumRectanglePoints);
    const Point3D referencePoint(referencePointAndAngleInRadians.first.x, referencePointAndAngleInRadians.first.y, minimumZ);

    if (qFuzzyCompare(referencePointAndAngleInRadians.second, 0.0))
    {
        for (auto &volume : volumes)
        {
            for (auto &point : volume.m_points)
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
            for (auto &point : volumes[static_cast<size_t>(i)].m_points)
            {
                CgalPoint3D pointCgal(point.x - referencePoint.x, point.y - referencePoint.y, point.z - referencePoint.z);
                auto pointCgalRotated = transformation(pointCgal);
                point.x = pointCgalRotated.x();
                point.y = pointCgalRotated.y();
                point.z = pointCgalRotated.z();
            }
        }
    }
}



} // namespace domain
} // namespace syntheticSeismic
