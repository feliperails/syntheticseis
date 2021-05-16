#include "EclipseGridSurface.h"
#include "EclipseGrid.h"
#include "EquationPlane.h"
#include "ExtractMinimumRectangle2D.h"
#include "ExtractVolumes.h"
#include "RotateVolumeCoordinate.h"
#include "../../geometry/src/Point3D.h"
#include <algorithm>
#include <map>
#include <QDebug>

using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace domain {

EclipseGridSurface::EclipseGridSurface(
        const std::shared_ptr<EclipseGrid> &eclipseGrid,
        const size_t numberOfCellsInX,
        const size_t numberOfCellsInY
    ) : m_eclipseGrid(eclipseGrid), m_numberOfCellsInX(numberOfCellsInX), m_numberOfCellsInY(numberOfCellsInY)
{

}

std::shared_ptr<EclipseGridSurface::Result> EclipseGridSurface::extractMainSurface() const
{
    const auto volumesFirstLayer = ExtractVolumes::extractFirstLayerFrom(*m_eclipseGrid);
    const auto allVolumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesFirstLayer, *m_eclipseGrid);

    size_t mainSurfaceVolumesSize = m_eclipseGrid->numberOfCellsInX() * m_eclipseGrid->numberOfCellsInY();

    const std::vector<std::shared_ptr<Volume>> volumesAux(allVolumes.begin(), allVolumes.begin() + mainSurfaceVolumesSize);
    std::vector<std::shared_ptr<Volume>> volumes;
    volumes.reserve(volumesAux.size());
    for (auto& item : volumesAux)
    {
        if (item->actnum)
        {
            volumes.push_back(item);
        }
    }

    return extractFromVolumes(volumes, 0);
}

std::vector<std::shared_ptr<EclipseGridSurface::Result>> EclipseGridSurface::extractSurfacesByAge(
        bool rotateVolumes,
        int ageMultiplicationFactorToIdentifyAges
    ) const
{
    const auto volumesFirstLayer = ExtractVolumes::extractFirstLayerFrom(*m_eclipseGrid);
    auto allVolumes = ExtractVolumes::extractFromVolumesOfFirstLayer(volumesFirstLayer, *m_eclipseGrid);
    const auto allVolumesSize = allVolumes.size();
    double angle = 0.0;
    Point3D referencePoint;

    if (rotateVolumes)
    {
        const auto minimumRectangle = ExtractMinimumRectangle2D::extractFrom(allVolumes);
        const auto rotateResult = RotateVolumeCoordinate::rotateByMinimumRectangle(allVolumes, minimumRectangle);
        angle = rotateResult->angle;
        referencePoint = rotateResult->origin;
    }

    size_t volumesSizeBySurface = m_eclipseGrid->numberOfCellsInX() * m_eclipseGrid->numberOfCellsInY();
    std::map<int, std::vector<std::shared_ptr<Volume>>> surfacesByAge;
    for (const auto &volume : allVolumes)
    {
        if (volume->actnum)
        {
            const int age = volume->age * std::pow(10.0, ageMultiplicationFactorToIdentifyAges);
            if (surfacesByAge.find(age) == surfacesByAge.end())
            {
                surfacesByAge[age] = std::vector<std::shared_ptr<Volume>>(volumesSizeBySurface);
            }
            const size_t indexLinear = volume->positionX + volume->positionY * m_eclipseGrid->numberOfCellsInX();
            surfacesByAge[age][indexLinear] = volume;
        }
    }

    std::vector<int> agesList;
    for (const auto &item : surfacesByAge)
    {
        agesList.push_back(item.first);
    }
    std::sort(agesList.begin(), agesList.end());
    std::map<int, size_t> mapAgesList;
    for (size_t i = 0; i < agesList.size(); ++i)
    {
        mapAgesList[agesList[i]] = i;
    }

    for (auto &surfaceByAge : surfacesByAge)
    {
        for (size_t indexLinear = 0; indexLinear < volumesSizeBySurface; ++indexLinear)
        {
            if (surfaceByAge.second[indexLinear] == nullptr)
            {
                for (size_t previousAgeIndexAux = mapAgesList[surfaceByAge.first]; previousAgeIndexAux > 0; --previousAgeIndexAux)
                {
                    const size_t previousAgeIndex = previousAgeIndexAux;

                    const int previousAge = agesList[previousAgeIndex];
                    if (surfacesByAge[previousAge][indexLinear] != nullptr)
                    {
                        surfaceByAge.second[indexLinear] = surfacesByAge[previousAge][indexLinear];
                        break;
                    }
                }
            }
        }
    }

    std::vector<std::shared_ptr<EclipseGridSurface::Result>> results;
    results.reserve(surfacesByAge.size());
    
    for (const auto& surfaceByAge : surfacesByAge)
    {
        const auto result = extractFromVolumes(surfaceByAge.second, surfaceByAge.first);
        result->getSurface()->setAngle(angle);
        result->getSurface()->setReferencePoint(referencePoint);
        
        result->getLithologySurface()->setAngle(angle);
        result->getLithologySurface()->setReferencePoint(referencePoint);
        
        result->getFaciesAssociationSurface()->setAngle(angle);
        result->getFaciesAssociationSurface()->setReferencePoint(referencePoint);
        
        result->getAgeSurface()->setAngle(angle);
        result->getAgeSurface()->setReferencePoint(referencePoint);

        results.push_back(result);
    }

    return results;
}

std::shared_ptr<EclipseGridSurface::Result> EclipseGridSurface::extractFromVolumes(const std::vector<std::shared_ptr<Volume>> &volumes, double age) const
{
    auto result = std::make_shared<EclipseGridSurface::Result>(
                std::make_shared<GrdSurface<double>>(m_numberOfCellsInX, m_numberOfCellsInY),
                std::make_shared<GrdSurface<int>>(m_numberOfCellsInX, m_numberOfCellsInY),
                std::make_shared<GrdSurface<int>>(m_numberOfCellsInX, m_numberOfCellsInY),
                std::make_shared<GrdSurface<double>>(m_numberOfCellsInX, m_numberOfCellsInY),
                age
            );
    auto &data = result->getSurface()->getData();
    auto& lithologyData = result->getLithologySurface()->getData();
    auto& faciesAssociationData = result->getFaciesAssociationSurface()->getData();
    auto& ageData = result->getAgeSurface()->getData();

    const size_t numberOfPointsOfFirstPlane = 4;
    const std::vector<std::vector<size_t>> trianglesPoints = {{0, 1, 2}, {2,3,1}};

    double grdXMin = std::numeric_limits<double>::max();
    double grdYMin = std::numeric_limits<double>::max();
    double grdXMax = -std::numeric_limits<double>::max();
    double grdYMax = -std::numeric_limits<double>::max();

    for (const auto &volume : volumes)
    {
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

    result->getSurface()->setXMin(grdXMin);
    result->getSurface()->setYMin(grdYMin);
    result->getSurface()->setXMax(grdXMax);
    result->getSurface()->setYMax(grdYMax);

    result->getLithologySurface()->setXMin(grdXMin);
    result->getLithologySurface()->setYMin(grdYMin);
    result->getLithologySurface()->setXMax(grdXMax);
    result->getLithologySurface()->setYMax(grdYMax);
    
    result->getAgeSurface()->setXMin(grdXMin);
    result->getAgeSurface()->setYMin(grdYMin);
    result->getAgeSurface()->setXMax(grdXMax);
    result->getAgeSurface()->setYMax(grdYMax);
    
    result->getFaciesAssociationSurface()->setXMin(grdXMin);
    result->getFaciesAssociationSurface()->setYMin(grdYMin);
    result->getFaciesAssociationSurface()->setXMax(grdXMax);
    result->getFaciesAssociationSurface()->setYMax(grdYMax);
    
    const double cellSizeX = (grdXMax - grdXMin) / m_numberOfCellsInX;
    const double cellSizeY = (grdYMax - grdYMin) / m_numberOfCellsInY;

    CgalKernel cgalKernel;

    for (const auto &volume : volumes)
    {
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
            const auto positionEndX = std::min(static_cast<size_t>((maxX - grdXMin) / cellSizeX), m_numberOfCellsInX - 1);
            const auto positionStartY = static_cast<size_t>((minY - grdYMin) / cellSizeY);
            const auto positionEndY = std::min(static_cast<size_t>((maxY - grdYMin) / cellSizeY), m_numberOfCellsInY - 1);

            for (size_t positionX = positionStartX; positionX <= positionEndX; ++positionX)
            {
                for (size_t positionY = positionStartY; positionY <= positionEndY; ++positionY)
                {
                    if (lithologyData[positionX][positionY] != GrdSurface<int>::NoDataValue
                        || faciesAssociationData[positionX][positionY] != GrdSurface<int>::NoDataValue)
                    {
                        continue;
                    }

                    const double x = grdXMin + positionX * cellSizeX + cellSizeX / 2.0;
                    const double y = grdYMin + positionY * cellSizeY + cellSizeX / 2.0;
                    CgalPoint cgalPoint(x, y);

                    if (checkInside(cgalPoint, cgalPoints, cgalPoints + 3, cgalKernel))
                    {
                        lithologyData[positionX][positionY] = volume->idLithology;
                        faciesAssociationData[positionX][positionY] = volume->idFaciesAssociation;
                        data[positionX][positionY] = equationPlane.calculateZByXY(x, y);
                        ageData[positionX][positionY] = volume->age;
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

EclipseGridSurface::Result::Result(
        const std::shared_ptr<GrdSurface<double>>& surface,
        const std::shared_ptr<GrdSurface<int>>& lithologySurface,
        const std::shared_ptr<GrdSurface<int>>& faciesAssociationSurface,
        const std::shared_ptr<GrdSurface<double>>& ageSurface,
        const int age
    )
    : m_surface(surface)
    , m_lithologySurface(lithologySurface)
    , m_faciesAssociationSurface(faciesAssociationSurface)
    , m_ageSurface(ageSurface)
    , m_age(age)
{

}

std::shared_ptr<GrdSurface<int>> EclipseGridSurface::Result::getLithologySurface()
{
    return m_lithologySurface;
}

std::shared_ptr<GrdSurface<int>> EclipseGridSurface::Result::getFaciesAssociationSurface()
{
    return m_faciesAssociationSurface;
}

int EclipseGridSurface::Result::getAge() const
{
    return m_age;
}

std::shared_ptr<GrdSurface<double>> EclipseGridSurface::Result::getSurface()
{
    return m_surface;
}

std::shared_ptr<GrdSurface<double>> EclipseGridSurface::Result::getAgeSurface()
{
    return m_ageSurface;
}

} // namespace domain
} // namespace syntheticSeismic
