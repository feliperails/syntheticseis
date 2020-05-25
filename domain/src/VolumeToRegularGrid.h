#pragma once

#include "ExtractVolumes.h"
#include "RegularGrid.h"
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
    static const size_t UNDEFINED_LITHOLOGY = std::numeric_limits<size_t>::max();

    typedef std::tuple<std::pair<size_t, size_t>, std::pair<size_t, size_t>, std::pair<size_t, size_t>> BoundaryBoxIndex;
    typedef CGAL::Simple_cartesian<double> CgalKernel;
    typedef CgalKernel::Point_3 CgalPoint3D;
    typedef CGAL::Polyhedron_3<CgalKernel> CgalPolyhedron3D;
    typedef CGAL::AABB_face_graph_triangle_primitive<CgalPolyhedron3D> CgalPolyhedronPrimitive3D;
    typedef CGAL::AABB_traits<CgalKernel, CgalPolyhedronPrimitive3D> CgalPolyhedronTraits3D;
    typedef CGAL::AABB_tree<CgalPolyhedronTraits3D> CgalPolyhedronTree3D;
    typedef CGAL::Side_of_triangle_mesh<CgalPolyhedron3D, CgalKernel> CgalPolyhedronPointInside3D;

    VolumeToRegularGrid(size_t numberOfCellsInX, size_t numberOfCellsInY, size_t numberOfCellsInZ);

    RegularGrid<size_t> convertVolumesToRegularGrid(std::vector<Volume> volumes);

    bool getBreakInFirstColision() const;
    void setBreakInFirstColision(bool breakInFirstColision);

private:
    size_t m_numberOfCellsInX = 0;
    size_t m_numberOfCellsInY = 0;
    size_t m_numberOfCellsInZ = 0;
    double m_cellSizeInX = 0.0;
    double m_cellSizeInY = 0.0;
    double m_cellSizeInZ = 0.0;
    bool m_breakInFirstColision = true;

    BoundaryBoxIndex calculateBoundaryBoxIndex(std::vector<Point3D> points);
};

} // namespace domain
} // namespace syntheticSeismic
