#pragma once
#include <vector>
#include "GrdSurface.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

namespace syntheticSeismic {

namespace geometry {
class Point3D;
class Volume;
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
        explicit Result(
                const std::shared_ptr<GrdSurface<double>>& surface,
                const std::shared_ptr<GrdSurface<int>>& lithologySurface,
                const std::shared_ptr<GrdSurface<int>>& faciesAssociationSurface,
                const std::shared_ptr<GrdSurface<double>>& ageSurface,
                const int age
            );

        std::shared_ptr<GrdSurface<double>> getSurface();

        std::shared_ptr<GrdSurface<int>> getLithologySurface();

        std::shared_ptr<GrdSurface<int>> getFaciesAssociationSurface();

        std::shared_ptr<GrdSurface<double>> getAgeSurface();

        int getAge() const;
    private:
        std::shared_ptr<GrdSurface<double>> m_surface;
        std::shared_ptr<GrdSurface<int>> m_lithologySurface;
        std::shared_ptr<GrdSurface<int>> m_faciesAssociationSurface;
        std::shared_ptr<GrdSurface<double>> m_ageSurface;
        int m_age;
    };

    EclipseGridSurface(
            const std::shared_ptr<EclipseGrid> &eclipseGrid,
            const size_t numberOfCellsInX,
            const size_t numberOfCellsInY
        );

    std::shared_ptr<Result> extractMainSurface() const;

    std::vector<std::shared_ptr<Result>> extractSurfacesByAge(bool rotateVolumes, int ageMultiplicationFactorToIdentifyAges) const;

    std::shared_ptr<Result> extractFromVolumes(const std::vector<std::shared_ptr<geometry::Volume>> &volumes, double age) const;

    bool checkInside(CgalPoint point, CgalPoint *polygonBegin, CgalPoint *polygonEnd, CgalKernel traits) const;

    geometry::Point3D crossProduct(const geometry::Point3D &p, const geometry::Point3D &q, const geometry::Point3D &r);
private:
    const std::shared_ptr<EclipseGrid> m_eclipseGrid;
    const size_t m_numberOfCellsInX;
    const size_t m_numberOfCellsInY;
};

} // namespace domain
} // namespace syntheticSeismic
