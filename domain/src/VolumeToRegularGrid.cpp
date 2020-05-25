#include "VolumeToRegularGrid.h"
#include <omp.h>

#define TUPLE_INDEX_X 0
#define TUPLE_INDEX_Y 1
#define TUPLE_INDEX_Z 2
#define PROCESS_VOLUME_CHUNK 1
#define PROCESS_MIN_MAX_CHUNK 10000

namespace syntheticSeismic {
namespace domain {

VolumeToRegularGrid::VolumeToRegularGrid(size_t numberOfCellsInX, size_t numberOfCellsInY, size_t numberOfCellsInZ) :
    m_numberOfCellsInX(numberOfCellsInX),
    m_numberOfCellsInY(numberOfCellsInY),
    m_numberOfCellsInZ(numberOfCellsInZ)
{

}

RegularGrid<size_t> VolumeToRegularGrid::convertVolumesToRegularGrid(
        std::vector<Volume> volumes
    )
{
    const std::vector<std::vector<size_t>> indexesTetraedronsInHexaedron = {
        {2, 0, 1, 5},
        {5, 4, 6, 2},
        {5, 4, 0, 2},
        {1, 3, 2, 6},
        {6, 7, 5, 1},
        {7, 6, 3, 1}
    };

    int volumesSize = static_cast<int>(volumes.size());

    double maxDouble = std::numeric_limits<double>::max();
    double maxX = -maxDouble;
    double maxY = -maxDouble;
    double maxZ = -maxDouble;

    #pragma omp parallel for schedule(dynamic, PROCESS_MIN_MAX_CHUNK)
    for (int i = 0; i < volumesSize; ++i)
    {
        const auto indexVolume = static_cast<size_t>(i);

        for (size_t j = 0; j < volumes[indexVolume].m_points.size(); ++j)
        {
            const auto &point = volumes[indexVolume].m_points[j];
            if (point.x > maxX)
            {
                maxX = point.x;
            }

            if (point.y > maxY)
            {
                maxY = point.y;
            }

            if (point.z > maxZ)
            {
                maxZ = point.z;
            }
        }
    }

    m_cellSizeInX = maxX / m_numberOfCellsInX;
    m_cellSizeInY = maxY / m_numberOfCellsInY;
    m_cellSizeInZ = maxZ / m_numberOfCellsInZ;

    RegularGrid<size_t> regularGrid(
                m_numberOfCellsInX, m_numberOfCellsInY, m_numberOfCellsInZ,
                m_cellSizeInX, m_cellSizeInY, m_cellSizeInZ,
                UNDEFINED_LITHOLOGY
            );
    auto &regularGridData = regularGrid.getData();

    #pragma omp parallel for schedule(dynamic, PROCESS_VOLUME_CHUNK)
    for (int i = 0; i < volumesSize; ++i)
    {
        const auto indexVolume = static_cast<size_t>(i);
        const auto &volume = volumes[indexVolume];

        const auto &points = volume.m_points;
        for (size_t indexTetraedron = 0; indexTetraedron < indexesTetraedronsInHexaedron.size(); ++indexTetraedron)
        {
            const auto &indexes = indexesTetraedronsInHexaedron[indexTetraedron];

            CgalPolyhedron3D polyhedron;
            polyhedron.make_tetrahedron(
                        CgalPoint3D(points[indexes[0]].x, points[indexes[0]].y, points[indexes[0]].z),
                        CgalPoint3D(points[indexes[1]].x, points[indexes[1]].y, points[indexes[1]].z),
                        CgalPoint3D(points[indexes[2]].x, points[indexes[2]].y, points[indexes[2]].z),
                        CgalPoint3D(points[indexes[3]].x, points[indexes[3]].y, points[indexes[3]].z)
                    );

            CgalPolyhedronTree3D tree(faces(polyhedron).first, faces(polyhedron).second, polyhedron);
            tree.accelerate_distance_queries();
            CgalPolyhedronPointInside3D pointInside(tree);

            const auto boundaryBoxIndex = calculateBoundaryBoxIndex(
                    {points[indexes[0]], points[indexes[1]], points[indexes[2]], points[indexes[3]]}
                );

            size_t indexXMax = std::get<TUPLE_INDEX_X>(boundaryBoxIndex).second;
            size_t indexYMax = std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).second;
            size_t indexZMax = std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).second;

            for (size_t indexX = std::get<TUPLE_INDEX_X>(boundaryBoxIndex).first; indexX <= indexXMax; ++indexX)
            {
                for (size_t indexY = std::get<TUPLE_INDEX_Y>(boundaryBoxIndex).first; indexY <= indexYMax; ++indexY)
                {
                    for (size_t indexZ = std::get<TUPLE_INDEX_Z>(boundaryBoxIndex).first; indexZ <= indexZMax; ++indexZ)
                    {
                        if (regularGridData[indexX][indexY][indexZ] != UNDEFINED_LITHOLOGY)
                        {
                            if (m_breakInFirstColision)
                            {
                                continue;
                            }
                            else if (regularGridData[indexX][indexY][indexZ] >= indexVolume)
                            {
                                continue;
                            }
                        }

                        [&] {
                            for (double incrementX = 0.0; incrementX <= 0.5; incrementX += 0.5)
                            {
                                for (double incrementY = 0.0; incrementY <= 0.5; incrementY += 0.5)
                                {
                                    for (double incrementZ = 0.0; incrementZ <= 0.5; incrementZ += 0.5)
                                    {
                                        const auto findPoint = CgalPoint3D(
                                                    m_cellSizeInX * (indexX + incrementX),
                                                    m_cellSizeInY * (indexY + incrementY),
                                                    m_cellSizeInZ * (indexZ + incrementZ)
                                                );

                                        if (pointInside(findPoint) == CGAL::ON_BOUNDED_SIDE)
                                        {
                                            regularGridData[indexX][indexY][indexZ] = indexVolume;
                                            return;
                                        }
                                    }
                                }
                            }
                        }();
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

void VolumeToRegularGrid::setBreakInFirstColision(bool breakInFirstColision)
{
    m_breakInFirstColision = breakInFirstColision;
}

VolumeToRegularGrid::BoundaryBoxIndex VolumeToRegularGrid::calculateBoundaryBoxIndex(std::vector<Point3D> points)
{
    size_t indexX = static_cast<size_t>(points[0].x / m_cellSizeInX);
    size_t indexY = static_cast<size_t>(points[0].y / m_cellSizeInY);
    size_t indexZ = static_cast<size_t>(points[0].z / m_cellSizeInZ);

    BoundaryBoxIndex boundaryBoxIndex = {
        {indexX, indexX},
        {indexY, indexY},
        {indexZ, indexZ}
    };

    for (size_t i = 1; i < points.size(); ++i)
    {
        const auto &point = points[i];

        size_t indexX = static_cast<size_t>(point.x / m_cellSizeInX);
        size_t indexY = static_cast<size_t>(point.y / m_cellSizeInY);
        size_t indexZ = static_cast<size_t>(point.z / m_cellSizeInZ);

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
