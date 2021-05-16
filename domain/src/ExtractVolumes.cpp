#include "ExtractVolumes.h"
#include "DomainConstant.h"
#include "geometry/src/Point2D.h"
#include "geometry/src/Volume.h"
#include <assert.h>

using namespace syntheticSeismic::geometry;

namespace syntheticSeismic {
namespace domain {

std::vector<std::shared_ptr<Volume>> ExtractVolumes::extractFirstLayerFrom(const syntheticSeismic::domain::EclipseGrid& eclipseGrid)
{
    const auto numberOfPointsInFirstLayerOfVolume = DomainConstant::NumberOfPointsInFirstLayerOfAnEclipseGridVolume;
    const size_t volumeCount = eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY();

    std::vector<std::shared_ptr<Volume>> volumes(volumeCount);
    for (size_t i = 0; i < volumeCount; ++i)
    {
        volumes[i] = std::make_shared<Volume>(i);
    }

    const auto amountOfVerticalLines = static_cast<int>(eclipseGrid.numberOfCellsInY());
    const auto amountOfHorizontalLines = eclipseGrid.numberOfCellsInX();

    for (int verticalLineIndexAux = 0; verticalLineIndexAux < amountOfVerticalLines; ++verticalLineIndexAux)
    {
        const size_t verticalLineIndex = static_cast<size_t>(verticalLineIndexAux);
        for (size_t horizontalLineIndex = 0; horizontalLineIndex < amountOfHorizontalLines; ++horizontalLineIndex)
        {
            const size_t volumeLinearIndex = horizontalLineIndex + verticalLineIndex * amountOfHorizontalLines;

            // Points of the first horizontal surface of the volume
            for (size_t pointIndexSurface = 0; pointIndexSurface < numberOfPointsInFirstLayerOfVolume; ++pointIndexSurface)
            {
                const auto pointIndexReordered = calculateReorderedIndex(pointIndexSurface);
                const auto eclipseGridCoordPointIndex = verticalLineIndex * ((amountOfHorizontalLines + 1) * 2) + horizontalLineIndex * 2;
                const auto indexCoord = eclipseGridCoordPointIndex + pointIndexSurface;
                const auto &coordinate = eclipseGrid.coordinates().at(indexCoord);
                volumes[volumeLinearIndex]->points[pointIndexReordered].x = coordinate.x();
                volumes[volumeLinearIndex]->points[pointIndexReordered].y = coordinate.y();
                volumes[volumeLinearIndex]->points[pointIndexReordered].z = coordinate.z();
            }
            // Points of the second horizontal surface of the volume
            for (size_t pointIndexSurface = 0; pointIndexSurface < numberOfPointsInFirstLayerOfVolume; ++pointIndexSurface)
            {
                const auto pointIndexReordered = calculateReorderedIndex(numberOfPointsInFirstLayerOfVolume + pointIndexSurface);
                const auto eclipseGridCoordPointIndex = (verticalLineIndex + 1) * ((amountOfHorizontalLines + 1) * 2) + horizontalLineIndex * 2;
                const auto indexCoord = eclipseGridCoordPointIndex + pointIndexSurface;
                const auto &coordinate = eclipseGrid.coordinates().at(indexCoord);
                volumes[volumeLinearIndex]->points[pointIndexReordered].x = coordinate.x();
                volumes[volumeLinearIndex]->points[pointIndexReordered].y = coordinate.y();
                volumes[volumeLinearIndex]->points[pointIndexReordered].z = coordinate.z();
            }
        }
    }

    return volumes;
}

std::vector<std::shared_ptr<Volume>> ExtractVolumes::extractFromVolumesOfFirstLayer(const std::vector<std::shared_ptr<Volume>> &volumesOfFirstLayer,
                                                                   const syntheticSeismic::domain::EclipseGrid& eclipseGrid,
                                                                   const bool divideXandYIntoZ)
{
    const size_t numberOfPointsBetweenTwoPlanes = DomainConstant::NumberOfPointsBetweenTwoPlanesOfEclipseGridVolume;

    std::vector<std::vector<Point2D>> coordinateDifferenceXY(volumesOfFirstLayer.size());
    const auto coordinateDifferenceXYSize = coordinateDifferenceXY.size();

    assert(eclipseGrid.numberOfCellsInZ() > 0);
    const double divideZ = divideXandYIntoZ ? static_cast<double>(eclipseGrid.numberOfCellsInZ()) : 1;
    for (size_t i = 0; i < coordinateDifferenceXYSize; ++i)
    {
        auto &points = coordinateDifferenceXY[i];
        points.resize(numberOfPointsBetweenTwoPlanes);

        for (size_t indexPoint = 0; indexPoint < numberOfPointsBetweenTwoPlanes; ++indexPoint)
        {
            points[indexPoint].x = (volumesOfFirstLayer[i]->points[indexPoint + numberOfPointsBetweenTwoPlanes].x
                    - volumesOfFirstLayer[i]->points[indexPoint].x) / divideZ;
            points[indexPoint].y = (volumesOfFirstLayer[i]->points[indexPoint + numberOfPointsBetweenTwoPlanes].y
                    - volumesOfFirstLayer[i]->points[indexPoint].y) / divideZ;
        }
    }

    const auto numberOfCellsInX = eclipseGrid.numberOfCellsInX();
    const auto numberOfCellsInY = eclipseGrid.numberOfCellsInY();
    const auto numberOfCellsInZ = eclipseGrid.numberOfCellsInZ();
    std::vector<std::shared_ptr<Volume>> volumes(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);

    size_t volumesSizeByPlane = numberOfCellsInX * numberOfCellsInY;
    for (size_t i = 0; i < volumes.size(); ++i)
    {
        size_t positionX = i % volumesSizeByPlane % numberOfCellsInX;
        size_t positionY = i % volumesSizeByPlane / numberOfCellsInX;
        volumes[i] = std::make_shared<Volume>(i, positionX, positionY);
    }
    size_t indexVolume = 0;
    size_t indexZCorn = 0;

    for (int indexLayerZ = 0; indexLayerZ < static_cast<int>(numberOfCellsInZ); ++indexLayerZ)
    {
        const size_t indexVolumeStartFrontBack = indexVolume;
        // Indicates whether the points in the front layer or the back layer of the volumes are being calculated.
        // frontBack = 0 -> Front layer
        // frontBack = 1 -> Back layer
        for (size_t frontBack = 0; frontBack <= 1; ++frontBack)
        {
            indexVolume = indexVolumeStartFrontBack;
            const size_t kStartFrontBack = frontBack * 4;
            size_t indexVolumeLine = 0;
            for (size_t indexLayerY = 0; indexLayerY < numberOfCellsInY; ++indexLayerY)
            {
                for (size_t kStart = 0; kStart <= 2; kStart += 2)
                {
                    for (size_t indexVolumeCurrent = 0; indexVolumeCurrent < numberOfCellsInX; ++indexVolumeCurrent)
                    {
                        const size_t indexVolumeLoop = indexVolume + indexVolumeCurrent;
                        volumes[indexVolumeLoop]->idLithology = eclipseGrid.lithologyIds()[indexVolumeLoop];
                        volumes[indexVolumeLoop]->idFaciesAssociation = eclipseGrid.faciesAssociationIds()[indexVolumeLoop];
                        volumes[indexVolumeLoop]->actnum = eclipseGrid.actnums()[indexVolumeLoop];
                        volumes[indexVolumeLoop]->age = eclipseGrid.ages()[indexVolumeLoop];
                        const size_t indexVolumeMainPlanLoop = indexVolumeLine + indexVolumeCurrent;
                        for (size_t k = 0; k < 2; ++k)
                        {
                            const size_t indexPointLoop = kStartFrontBack + kStart + k;
                            const size_t indexPointDifferenceLoop = kStart + k;
                            volumes[indexVolumeLoop]->points[indexPointLoop].x =
                                    volumesOfFirstLayer[indexVolumeMainPlanLoop]->points[indexPointDifferenceLoop].x +
                                    coordinateDifferenceXY[indexVolumeMainPlanLoop][indexPointDifferenceLoop].x *
                                    (static_cast<size_t>(indexLayerZ) + frontBack);
                            volumes[indexVolumeLoop]->points[indexPointLoop].y =
                                    volumesOfFirstLayer[indexVolumeMainPlanLoop]->points[indexPointDifferenceLoop].y +
                                    coordinateDifferenceXY[indexVolumeMainPlanLoop][indexPointDifferenceLoop].y *
                                    (static_cast<size_t>(indexLayerZ) + frontBack);
                            volumes[indexVolumeLoop]->points[indexPointLoop].z = eclipseGrid.zCoordinates()[indexZCorn];

                            ++indexZCorn;
                        }
                    }
                }
                indexVolumeLine += numberOfCellsInX;
                indexVolume += numberOfCellsInX;
            }
        }
    }

    return volumes;
}

}
}
