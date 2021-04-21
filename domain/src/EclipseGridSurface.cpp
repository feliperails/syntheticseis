#include "EclipseGridSurface.h"
#include "EclipseGrid.h"
#include "EquationPlane.h"
#include "ExtractVolumes.h"
#include "../../geometry/src/Point3D.h"
#include <algorithm>
#include <QDebug>

using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace domain {

EclipseGridSurface::Result EclipseGridSurface::extractFromMainSurface(
        const EclipseGrid &eclipseGrid, const size_t numberOfCellsInX, const size_t numberOfCellsInY
    ) const
{
    EclipseGridSurface::Result result(
                GrdSurface<double>(numberOfCellsInX, numberOfCellsInY),
                GrdSurface<int>(numberOfCellsInX, numberOfCellsInY)
            );
    auto &data = result.getSurface().getData();
    auto &lithologyData = result.getLithologySurface().getData();

    const auto volumesFirstLayer = ExtractVolumes::extractFirstLayerFrom(eclipseGrid);
    const auto volumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesFirstLayer, eclipseGrid);

    const size_t numberOfPointsOfFirstPlane = 4;
    const std::vector<std::vector<size_t>> trianglesPoints = {{0, 1, 2}, {2,3,1}};

    double grdXMin = std::numeric_limits<double>::max();
    double grdYMin = std::numeric_limits<double>::max();
    double grdXMax = -std::numeric_limits<double>::max();
    double grdYMax = -std::numeric_limits<double>::max();

    size_t mainSurfaceVolumesSize = eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY();
    for (size_t indexVolume = 0; indexVolume < mainSurfaceVolumesSize; ++indexVolume)
    {
        const auto &volume = volumes[indexVolume];

        for (size_t i = 0; i < numberOfPointsOfFirstPlane; ++i)
        {
            if (grdXMin > volume->points[i].x)
            {
                grdXMin = volume->points[i].x;
            }
            if (grdYMin > volume->points[i].y)
            {
                grdYMin = volume->points[i].y;
            }
            if (grdXMax < volume->points[i].x)
            {
                grdXMax = volume->points[i].x;
            }
            if (grdYMax < volume->points[i].y)
            {
                grdYMax = volume->points[i].y;
            }
        }
    }

    result.getSurface().setXMin(grdXMin);
    result.getSurface().setYMin(grdYMin);
    result.getSurface().setXMax(grdXMax);
    result.getSurface().setYMax(grdYMax);
    result.getLithologySurface().setXMin(grdXMin);
    result.getLithologySurface().setYMin(grdYMin);
    result.getLithologySurface().setXMax(grdXMax);
    result.getLithologySurface().setYMax(grdYMax);
    double cellSizeX = (grdXMax - grdXMin) / numberOfCellsInX;
    double cellSizeY = (grdYMax - grdYMin) / numberOfCellsInY;

    CgalKernel cgalKernel;

    for (size_t indexVolume = 0; indexVolume < mainSurfaceVolumesSize; ++indexVolume)
    {
        const auto &volume = volumes[indexVolume];
        const auto &points = volume->points;
        for (const auto &triangle : trianglesPoints)
        {
            double maxX = std::max(points[triangle[0]].x, std::max(points[triangle[1]].x, points[triangle[2]].x));
            double minX = std::min(points[triangle[0]].x, std::min(points[triangle[1]].x, points[triangle[2]].x));
            double maxY = std::max(points[triangle[0]].y, std::max(points[triangle[1]].y, points[triangle[2]].y));
            double minY = std::min(points[triangle[0]].y, std::min(points[triangle[1]].y, points[triangle[2]].y));

            CgalPoint cgalPoints[3] = {
                CgalPoint(points[triangle[0]].x, points[triangle[0]].y),
                CgalPoint(points[triangle[1]].x, points[triangle[1]].y),
                CgalPoint(points[triangle[2]].x, points[triangle[2]].y)
            };

            EquationPlane equationPlane(points[triangle[0]], points[triangle[1]], points[triangle[2]]);

            const auto positionStartX = static_cast<size_t>((minX - grdXMin) / cellSizeX);
            const auto positionEndX = std::min(static_cast<size_t>((maxX - grdXMin) / cellSizeX), numberOfCellsInX - 1);
            const auto positionStartY = static_cast<size_t>((minY - grdYMin) / cellSizeY);
            const auto positionEndY = std::min(static_cast<size_t>((maxY - grdYMin) / cellSizeY), numberOfCellsInY - 1);

            for (size_t positionX = positionStartX; positionX <= positionEndX; ++positionX)
            {
                for (size_t positionY = positionStartY; positionY <= positionEndY; ++positionY)
                {
                    if (lithologyData[positionX][positionY] != GrdSurface<int>::NoDataValue)
                    {
                        continue;
                    }

                    const double x = grdXMin + positionX * cellSizeX + cellSizeX / 2.0;
                    const double y = grdYMin + positionY * cellSizeY + cellSizeX / 2.0;
                    CgalPoint cgalPoint(x, y);

                    if (checkInside(cgalPoint, cgalPoints, cgalPoints + 3, cgalKernel))
                    {
                        lithologyData[positionX][positionY] = volume->idLithology;
                        data[positionX][positionY] = equationPlane.calculateZByXY(x, y);
                    }
                }
            }
        }
    }

    return result;
}

bool EclipseGridSurface::checkInside(
        EclipseGridSurface::CgalPoint point, EclipseGridSurface::CgalPoint *polygonBegin,
        EclipseGridSurface::CgalPoint *polygonEnd, EclipseGridSurface::CgalKernel traits
    ) const
{
    switch (CGAL::bounded_side_2(polygonBegin, polygonEnd, point, traits)) {
    case CGAL::ON_BOUNDED_SIDE:
    case CGAL::ON_BOUNDARY:
        return true;
    default:
        return false;
    }
}

Point3D EclipseGridSurface::crossProduct(const Point3D &p, const Point3D &q, const Point3D &r)
{
    const Point3D pq(q.x - p.x, q.y - p.y, q.z - p.z);
    const Point3D pr(r.x - p.x, r.y - p.y, r.z - p.z);

    return Point3D(
                pr.y * pq.z - pr.z * pq.y,
                pr.z * pq.x - pr.x * pq.z,
                pr.x * pq.y - pr.y * pq.x
            );
}

EclipseGridSurface::Result::Result(const GrdSurface<double> &surface, const GrdSurface<int> &lithologySurface) :
    m_surface(surface), m_lithologySurface(lithologySurface)
{

}

GrdSurface<int>& EclipseGridSurface::Result::getLithologySurface()
{
    return m_lithologySurface;
}

GrdSurface<double>& EclipseGridSurface::Result::getSurface()
{
    return m_surface;
}

} // namespace domain
} // namespace syntheticSeismic
