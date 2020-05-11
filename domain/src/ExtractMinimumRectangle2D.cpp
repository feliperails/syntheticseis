#include "ExtractMinimumRectangle2D.h"
#include "DomainConstant.h"
#include <CGAL/Cartesian.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/property_map.h>
#include <CGAL/random_convex_set_2.h>
#include <iostream>
#include <numeric>
#include <vector>

#define CONVERT_VOLUME_TO_POINTS_OPENMP_CHUNK 100

typedef CGAL::Exact_predicates_inexact_constructions_kernel CgalKernel;
typedef CgalKernel::Point_2 CgalPoint2D;
typedef CGAL::Convex_hull_traits_adapter_2<CgalKernel, CGAL::Pointer_property_map<CgalPoint2D>::type> CgalConvexHullTraits2D;
typedef CGAL::Polygon_2<CgalKernel> CgalPolygon2D;

namespace syntheticSeismic {
namespace domain {

std::vector<Point2D> extractMinimumRectangle2D::extractFrom(const std::vector<Volume> &volumes)
{
    const auto numberOfPointsInVolume = DomainConstant::NumberOfPointsInAnEclipseGridVolume;

    std::vector<CgalPoint2D> points(volumes.size() * numberOfPointsInVolume);

    const auto volumesSize = static_cast<int>(volumes.size());
    #pragma omp parallel for schedule(dynamic, CONVERT_VOLUME_TO_POINTS_OPENMP_CHUNK)
    for (int i = 0; i < volumesSize; ++i)
    {
        const auto indexVolume = static_cast<size_t>(i);
        for (size_t j = 0; j < numberOfPointsInVolume; ++j)
        {
            const auto &point = volumes[indexVolume].m_points[j];
            points[indexVolume * numberOfPointsInVolume + j] = CgalPoint2D(point.x, point.y);
        }
    }

    std::vector<size_t> indices(points.size()), out;
    std::iota(indices.begin(), indices.end(), 0);

    CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out), CgalConvexHullTraits2D(CGAL::make_property_map(points)));

    std::vector<CgalPoint2D> convexHullPoints(out.size());
    {
        size_t index = 0;
        for(size_t i : out)
        {
            convexHullPoints[index] = points[i];
            ++index;
        }
    }
    CgalPolygon2D minRectanglePolygon2D;
    CGAL::min_rectangle_2(convexHullPoints.begin(), convexHullPoints.end(), std::back_inserter(minRectanglePolygon2D));

    std::vector<Point2D> result(minRectanglePolygon2D.size());
    for (size_t i = 0; i < minRectanglePolygon2D.size(); ++i)
    {
        result[i] = {minRectanglePolygon2D.vertex(i).x(), minRectanglePolygon2D.vertex(i).y()};
    }

    return result;
}

} // namespace domain
} // namespace syntheticSeismic
