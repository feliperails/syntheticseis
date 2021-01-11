#pragma once

#include "ExtractVolumes.h"
#include "RegularGrid.h"
#include "geometry/src/Point3D.h"
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/algorithm.h>
#include <CGAL/Side_of_triangle_mesh.h>

namespace syntheticSeismic {
namespace domain {

class VolumeToRegularGrid
{
public:
    typedef std::tuple<std::pair<size_t, size_t>, std::pair<size_t, size_t>, std::pair<size_t, size_t>> BoundaryBoxIndex;
    typedef CGAL::Simple_cartesian<double> CgalKernel;
    typedef CgalKernel::Point_3 CgalPoint3D;
    typedef CGAL::Polyhedron_3<CgalKernel> CgalPolyhedron3D;
    typedef CGAL::AABB_face_graph_triangle_primitive<CgalPolyhedron3D> CgalPolyhedronPrimitive3D;
    typedef CGAL::AABB_traits<CgalKernel, CgalPolyhedronPrimitive3D> CgalPolyhedronTraits3D;
    typedef CGAL::AABB_tree<CgalPolyhedronTraits3D> CgalPolyhedronTree3D;
    typedef CGAL::Side_of_triangle_mesh<CgalPolyhedron3D, CgalKernel> CgalPolyhedronPointInside3D;

    VolumeToRegularGrid(const size_t numberOfCellsInX, const size_t numberOfCellsInY, const size_t numberOfCellsInZ);

    RegularGrid<std::shared_ptr<geometry::Volume>> convertVolumesToRegularGrid(
            const std::vector<std::shared_ptr<geometry::Volume>> volumes,
            const std::array<geometry::Point2D, 4> &minimumRectangle,
            const double zBottom,
            const double zTop
        );

    bool getBreakInFirstColision() const;
    void setBreakInFirstColision(const bool breakInFirstColision);

private:
    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
    double m_cellSizeInX;
    double m_cellSizeInY;
    double m_cellSizeInZ;
    bool m_breakInFirstColision;

    BoundaryBoxIndex calculateBoundaryBoxIndex(const std::vector<geometry::Point3D> &points);
};

} // namespace domain
} // namespace syntheticSeismic
