#pragma once
#include <vector>
#include "GrdSurface.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

namespace syntheticSeismic {

namespace geometry {
class Point3D;
}

namespace domain {
class EclipseGrid;

class EclipseGridSurface
{
public:
    typedef CGAL::Exact_predicates_inexact_constructions_kernel CgalKernel;
    typedef CgalKernel::Point_2 CgalPoint;
    typedef CGAL::Polygon_2<CgalKernel> Polygon_2;

    class Result
    {
    public:
        explicit Result(const GrdSurface<double> &surface, const GrdSurface<int> &lithologySurface);

        GrdSurface<double>& getSurface();

        GrdSurface<int>& getLithologySurface();
    private:
        GrdSurface<double> m_surface;
        GrdSurface<int> m_lithologySurface;
    };

    Result extractFromMainSurface(const EclipseGrid &eclipseGrid, const size_t numberOfCellsInX, const size_t numberOfCellsInY) const;

    bool checkInside(CgalPoint point, CgalPoint *polygonBegin, CgalPoint *polygonEnd, CgalKernel traits) const;

    geometry::Point3D crossProduct(const geometry::Point3D &p, const geometry::Point3D &q, const geometry::Point3D &r);
};

} // namespace domain
} // namespace syntheticSeismic
