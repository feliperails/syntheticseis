#include "VolumeToRegularGrid.h"
#include "domain/src/CellSizeCalculator.h"
#include <omp.h>

#define PROCESS_VOLUME_CHUNK 100

namespace {
const size_t TUPLE_INDEX_X = 0;
const size_t TUPLE_INDEX_Y = 1;
const size_t TUPLE_INDEX_Z = 2;
}


using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace domain {

VolumeToRegularGrid::VolumeToRegularGrid(const size_t numberOfCellsInX, const size_t numberOfCellsInY, const size_t numberOfCellsInZ) :
    m_numberOfCellsInX(numberOfCellsInX),
    m_numberOfCellsInY(numberOfCellsInY),
    m_numberOfCellsInZ(numberOfCellsInZ),
    m_cellSizeInX(0.0),
    m_cellSizeInY(0.0),
    m_cellSizeInZ(0.0),
    m_calculeCellSize(true),
    m_breakInFirstColision(false)
{

}

VolumeToRegularGrid::VolumeToRegularGrid(
                            const size_t& numberOfCellsInX,
                            const size_t& numberOfCellsInY,
                            const size_t& numberOfCellsInZ,
                            const double& cellSizeInX,
                            const double& cellSizeInY,
                            const double& cellSizeInZ
                        ) :
    m_numberOfCellsInX(numberOfCellsInX),
    m_numberOfCellsInY(numberOfCellsInY),
    m_numberOfCellsInZ(numberOfCellsInZ),
    m_cellSizeInX(cellSizeInX),
    m_cellSizeInY(cellSizeInY),
    m_cellSizeInZ(cellSizeInZ),
    m_calculeCellSize(false),
    m_breakInFirstColision(false)
{

}

RegularGrid<std::shared_ptr<Volume>> VolumeToRegularGrid::convertVolumesToRegularGrid(
        const std::vector<std::shared_ptr<Volume>> &volumes,
        const std::array<geometry::Point2D, 4> &minimumRectangle,
        const double zBottom,
        const double zTop
    )
{
    const std::vector<std::vector<size_t>> indexesTetrahedronsInHexahedron = {
        {2, 0, 1, 5},
        {5, 4, 6, 2},
        {5, 4, 0, 2},
        {1, 3, 2, 6},
        {6, 7, 5, 1},
        {7, 6, 3, 1}
    };

    const int volumesSize = static_cast<int>(volumes.size());

    if (m_calculeCellSize)
    {
        domain::CellSizeCalculator cellSizeCalculator(
                m_numberOfCellsInX,
                m_numberOfCellsInY,
                m_numberOfCellsInZ,
                1.0,
                volumes
            );

        m_cellSizeInX = cellSizeCalculator.getCellSizeInX();
        m_cellSizeInY = cellSizeCalculator.getCellSizeInY();
        m_cellSizeInZ = cellSizeCalculator.getCellSizeInZ();
    }

    RegularGrid<std::shared_ptr<Volume>> regularGrid(
                m_numberOfCellsInX, m_numberOfCellsInY, m_numberOfCellsInZ,
                m_cellSizeInX, m_cellSizeInY, m_cellSizeInZ,
                EnumUnit::Meters, EnumUnit::Meters, EnumUnit::Meters,
                minimumRectangle,
                zBottom,
                zTop,
                nullptr
            );
    auto &regularGridData = regularGrid.getData();

    #pragma omp parallel for schedule(dynamic, PROCESS_VOLUME_CHUNK)
    for (int i = 0; i < volumesSize; ++i)
    {
        const auto indexVolume = static_cast<size_t>(i);
        const auto volume = volumes[indexVolume];
        if (!volume->actnum)
        {
            continue;
        }

        const auto &points = volume->points;
        for (size_t indexTetrahedron = 0; indexTetrahedron < indexesTetrahedronsInHexahedron.size(); ++indexTetrahedron)
        {
            const auto &indexes = indexesTetrahedronsInHexahedron[indexTetrahedron];

            CgalPolyhedron3D polyhedron;
            polyhedron.make_tetrahedron(
                        CgalPoint3D(points[indexes[0]].x, points[indexes[0]].y, points[indexes[0]].z),
                        CgalPoint3D(points[indexes[1]].x, points[indexes[1]].y, points[indexes[1]].z),
                        CgalPoint3D(points[indexes[2]].x, points[indexes[2]].y, points[indexes[2]].z),
                        CgalPoint3D(points[indexes[3]].x, points[indexes[3]].y, points[indexes[3]].z)
                    );

            CgalPolyhedronTree3D tree(faces(polyhedron).first, faces(polyhedron).second, polyhedron);
            tree.accelerate_distance_queries();
            const CgalPolyhedronPointInside3D pointInside(tree);

            const auto boundaryBoxIndex = calculateBoundaryBoxIndex(
                    {points[indexes[0]], points[indexes[1]], points[indexes[2]], points[indexes[3]]}
                );

            const size_t indexXMax = std::get<TUPLE_INDEX_X>(boundaryBoxIndex).second;
            const size_t indexYMax = std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).second;
            const size_t indexZMax = std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).second;

            for (size_t indexX = std::get<TUPLE_INDEX_X>(boundaryBoxIndex).first; indexX <= indexXMax; ++indexX)
            {
                for (size_t indexY = std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).first; indexY <= indexYMax; ++indexY)
                {
                    for (size_t indexZ = std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).first; indexZ <= indexZMax; ++indexZ)
                    {
                        auto breakAll = false;
                        for (int incrementX = 0; incrementX <= 1; ++incrementX)
                        {
                            for (int incrementY = 0; incrementY <= 1; ++incrementY)
                            {
                                for (int incrementZ = 0; incrementZ <= 1; ++incrementZ)
                                {

                                    if (regularGridData[indexX][indexY][indexZ] != nullptr)
                                    {
                                        if (m_breakInFirstColision)
                                        {
                                            breakAll = true;
                                            break;
                                        }
                                        // Point located in the center of the volume has higher priority than points in the corners.
                                        else if (incrementX != 1 || incrementY != 1 || incrementZ != 1)
                                        {
                                            breakAll = true;
                                            break;
                                        }
                                    }

                                    const auto findPoint = CgalPoint3D(
                                                m_cellSizeInX * (static_cast<double>(indexX) + incrementX / 2.0),
                                                m_cellSizeInY * (static_cast<double>(indexY) + incrementY / 2.0),
                                                m_cellSizeInZ * (static_cast<double>(indexZ) + incrementZ / 2.0)
                                            );

                                    auto resultPointInside = pointInside(findPoint);
                                    if (resultPointInside == CGAL::ON_BOUNDED_SIDE)
                                    {
                                        regularGridData[indexX][indexY][indexZ] = volume;
                                        breakAll = true;
                                        break;
                                    }
                                }
                                if (breakAll)
                                {
                                    break;
                                }
                            }
                            if (breakAll)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return regularGrid;
}

bool VolumeToRegularGrid::getBreakInFirstColision() const
{
    return m_breakInFirstColision;
}

void VolumeToRegularGrid::setBreakInFirstColision(const bool breakInFirstColision)
{
    m_breakInFirstColision = breakInFirstColision;
}

VolumeToRegularGrid::BoundaryBoxIndex VolumeToRegularGrid::calculateBoundaryBoxIndex(const std::vector<Point3D> &points)
{
    const size_t indexX = static_cast<size_t>(points[0].x / m_cellSizeInX);
    const size_t indexY = static_cast<size_t>(points[0].y / m_cellSizeInY);
    const size_t indexZ = static_cast<size_t>(points[0].z / m_cellSizeInZ);

    BoundaryBoxIndex boundaryBoxIndex = {
        {indexX, indexX},
        {indexY, indexY},
        {indexZ, indexZ}
    };

    for (size_t i = 1; i < points.size(); ++i)
    {
        const auto &point = points[i];

        const size_t indexX = static_cast<size_t>(point.x / m_cellSizeInX);
        const size_t indexY = static_cast<size_t>(point.y / m_cellSizeInY);
        const size_t indexZ = static_cast<size_t>(point.z / m_cellSizeInZ);

        if (indexX < std::get<TUPLE_INDEX_X>(boundaryBoxIndex).first)
        {
            std::get<TUPLE_INDEX_X>(boundaryBoxIndex).first = indexX;
        }
        else if (indexX > std::get<TUPLE_INDEX_X>(boundaryBoxIndex).second)
        {
            std::get<TUPLE_INDEX_X>(boundaryBoxIndex).second = indexX;
        }

        if (indexY < std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).first)
        {
            std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).first = indexY;
        }
        else if (indexY > std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).second)
        {
            std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).second = indexY;
        }

        if (indexZ < std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).first)
        {
            std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).first = indexZ;
        }
        else if (indexZ > std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).second)
        {
            std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).second = indexZ;
        }
    }

    std::get<TUPLE_INDEX_X>(boundaryBoxIndex).first = std::min(m_numberOfCellsInX - 1, std::get<TUPLE_INDEX_X>(boundaryBoxIndex).first);
    std::get<TUPLE_INDEX_X>(boundaryBoxIndex).second = std::min(m_numberOfCellsInX - 1, std::get<TUPLE_INDEX_X>(boundaryBoxIndex).second);

    std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).first = std::min(m_numberOfCellsInY - 1, std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).first);
    std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).second = std::min(m_numberOfCellsInY - 1, std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).second);

    std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).first = std::min(m_numberOfCellsInZ - 1, std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).first);
    std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).second = std::min(m_numberOfCellsInZ - 1, std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).second);

    return boundaryBoxIndex;
}

} // namespace domain
} // namespace syntheticSeismic
