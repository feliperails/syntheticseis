#include "ExtractMinimumRectangle2D.h"
#include "DomainConstant.h"
#include "geometry/src/Volume.h"
#include <CGAL/Cartesian.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/property_map.h>
#include <CGAL/random_convex_set_2.h>
#include <numeric>
#include <vector>
#include <omp.h>

#define CONVERT_VOLUME_TO_POINTS_OPENMP_CHUNK 100

using namespace syntheticSeismic::geometry;

typedef CGAL::Exact_predicates_inexact_constructions_kernel CgalKernel;
typedef CgalKernel::Point_2 CgalPoint2D;
typedef CGAL::Convex_hull_traits_adapter_2<CgalKernel, CGAL::Pointer_property_map<CgalPoint2D>::type> CgalConvexHullTraits2D;
typedef CGAL::Polygon_2<CgalKernel> CgalPolygon2D;

namespace syntheticSeismic {
namespace domain {

std::array<Point2D, 4> ExtractMinimumRectangle2D::extractFrom(const std::vector<std::shared_ptr<Volume>> &volumes)
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
            const auto &point = volumes[indexVolume]->points[j];
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

    std::array<Point2D, 4> result;
    for (size_t i = 0; i < minRectanglePolygon2D.size(); ++i)
    {
        result[i].x = minRectanglePolygon2D.vertex(i).x();
        result[i].y = minRectanglePolygon2D.vertex(i).y();
    }

    return result;
}

} // namespace domain
} // namespace syntheticSeismic
