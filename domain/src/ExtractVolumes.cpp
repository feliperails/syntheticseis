#include "ExtractVolumes.h"
#include "DomainConstant.h"
#include "Point2D.h"
#include <omp.h>

namespace syntheticSeismic {
namespace domain {

std::vector<Volume> ExtractVolumes::extractFirstLayerFrom(const syntheticSeismic::domain::EclipseGrid& eclipseGrid)
{
    const auto numberOfPointsInFirstLayerOfVolume = DomainConstant::NumberOfPointsInFirstLayerOfAnEclipseGridVolume;
    const size_t volumeCount = eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY();

    std::vector<Volume> volumes(volumeCount);

    const auto amountOfVerticalLines = static_cast<int>(eclipseGrid.numberOfCellsInY());
    const auto amountOfHorizontalLines = eclipseGrid.numberOfCellsInX();

    #pragma omp parallel for
    for (int verticalLineIndexAux = 0; verticalLineIndexAux < amountOfVerticalLines; ++verticalLineIndexAux)
    {
        size_t verticalLineIndex = static_cast<size_t>(verticalLineIndexAux);
        for (size_t horizontalLineIndex = 0; horizontalLineIndex < amountOfHorizontalLines; ++horizontalLineIndex)
        {
            size_t volumeLinearIndex = horizontalLineIndex + verticalLineIndex * amountOfHorizontalLines;

            // Points of the first horizontal surface of the volume
            for (size_t pointIndexSurface = 0; pointIndexSurface < numberOfPointsInFirstLayerOfVolume; ++pointIndexSurface)
            {
                auto pointIndexReordered = calculateReorderedIndex(pointIndexSurface);
                auto eclipseGridCoordPointIndex = verticalLineIndex * ((amountOfHorizontalLines + 1) * 2) + horizontalLineIndex * 2;
                auto indexCoord = eclipseGridCoordPointIndex + pointIndexSurface;
                auto &coordinate = eclipseGrid.coordinates().at(indexCoord);
                volumes[volumeLinearIndex].m_points[pointIndexReordered].x = coordinate.x();
                volumes[volumeLinearIndex].m_points[pointIndexReordered].y = coordinate.y();
                volumes[volumeLinearIndex].m_points[pointIndexReordered].z = coordinate.z();
            }
            // Points of the second horizontal surface of the volume
            for (size_t pointIndexSurface = 0; pointIndexSurface < numberOfPointsInFirstLayerOfVolume; ++pointIndexSurface)
            {
                auto pointIndexReordered = calculateReorderedIndex(numberOfPointsInFirstLayerOfVolume + pointIndexSurface);
                auto eclipseGridCoordPointIndex = (verticalLineIndex + 1) * ((amountOfHorizontalLines + 1) * 2) + horizontalLineIndex * 2;
                auto indexCoord = eclipseGridCoordPointIndex + pointIndexSurface;
                auto &coordinate = eclipseGrid.coordinates().at(indexCoord);
                volumes[volumeLinearIndex].m_points[pointIndexReordered].x = coordinate.x();
                volumes[volumeLinearIndex].m_points[pointIndexReordered].y = coordinate.y();
                volumes[volumeLinearIndex].m_points[pointIndexReordered].z = coordinate.z();
            }
        }
    }

    return volumes;
}

std::vector<Volume> ExtractVolumes::extractFromVolumesOfFirstLayer(const std::vector<Volume> &volumesOfFirstLayer,
                                                                   const syntheticSeismic::domain::EclipseGrid& eclipseGrid)
{
    const size_t numberOfPointsBetweenTwoPlanes = DomainConstant::NumberOfPointsBetweenTwoPlanesOfEclipseGridVolume;

    std::vector<std::vector<Point2D>> coordinateDifferenceXY(volumesOfFirstLayer.size());
    std::vector<std::vector<Point2D>> coordinateDifferenceXYCurrent(volumesOfFirstLayer.size());
    auto coordinateDifferenceXYSize = coordinateDifferenceXYCurrent.size();
    for (size_t i = 0; i < coordinateDifferenceXYSize; ++i)
    {
        coordinateDifferenceXYCurrent[i].resize(numberOfPointsBetweenTwoPlanes);

        auto &points = coordinateDifferenceXY[i];
        points.resize(numberOfPointsBetweenTwoPlanes);
        for (size_t indexPoint = 0; indexPoint < numberOfPointsBetweenTwoPlanes; ++indexPoint)
        {
            points[indexPoint].x = volumesOfFirstLayer[i].m_points[indexPoint + numberOfPointsBetweenTwoPlanes].x
                    - volumesOfFirstLayer[i].m_points[indexPoint].x;
            points[indexPoint].y = volumesOfFirstLayer[i].m_points[indexPoint + numberOfPointsBetweenTwoPlanes].y
                    - volumesOfFirstLayer[i].m_points[indexPoint].y;
        }
    }

    auto numberOfCellsInX = eclipseGrid.numberOfCellsInX();
    auto numberOfCellsInY = eclipseGrid.numberOfCellsInY();
    auto numberOfCellsInZ = eclipseGrid.numberOfCellsInZ();
    std::vector<Volume> volumes(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);
    size_t indexVolume = 0;
    size_t indexZCorn = 0;
    for (int indexSliceZ = 0; indexSliceZ < static_cast<int>(numberOfCellsInZ); ++indexSliceZ)
    {
        size_t indexVolumeStartFrontBack = indexVolume;
        for (size_t frontBack = 0; frontBack <= 1; ++frontBack)
        {
            indexVolume = indexVolumeStartFrontBack;
            size_t kStartFrontBack = frontBack * 4;
            size_t indexVolumeLine = 0;
            for (size_t indexSliceY = 0; indexSliceY < numberOfCellsInY; ++indexSliceY)
            {
                for (size_t kStart = 0; kStart <= 2; kStart += 2)
                {
                    for (size_t indexVolumeCurrent = 0; indexVolumeCurrent < numberOfCellsInX; ++indexVolumeCurrent)
                    {
                        size_t indexVolumeLoop = indexVolume + indexVolumeCurrent;
                        size_t indexVolumeMainPlanLoop = indexVolumeLine + indexVolumeCurrent;
                        for (size_t k = 0; k < 2; ++k)
                        {
                            size_t indexPointLoop = kStartFrontBack + kStart + k;
                            size_t indexPointDifferenceLoop = kStart + k;
                            volumes[indexVolumeLoop].m_points[indexPointLoop].x =
                                    volumesOfFirstLayer[indexVolumeMainPlanLoop].m_points[indexPointLoop].x +
                                    coordinateDifferenceXYCurrent[indexVolumeMainPlanLoop][indexPointDifferenceLoop].x;
                            volumes[indexVolumeLoop].m_points[indexPointLoop].y =
                                    volumesOfFirstLayer[indexVolumeMainPlanLoop].m_points[indexPointLoop].y +
                                    coordinateDifferenceXYCurrent[indexVolumeMainPlanLoop][indexPointDifferenceLoop].y;
                            volumes[indexVolumeLoop].m_points[indexPointLoop].z = eclipseGrid.zCoordinates()[indexZCorn];
                            ++indexZCorn;
                        }
                    }
                }
                indexVolumeLine = indexVolumeLine + numberOfCellsInX;
                indexVolume = indexVolume + numberOfCellsInX;
            }

            for (size_t i = 0; i < coordinateDifferenceXYSize; ++i)
            {
                auto &pointsCurrent = coordinateDifferenceXYCurrent[i];
                auto &points = coordinateDifferenceXY[i];
                for (size_t indexPoint = 0; indexPoint < numberOfPointsBetweenTwoPlanes; ++indexPoint)
                {
                    pointsCurrent[indexPoint].x = pointsCurrent[indexPoint].x + points[indexPoint].x;
                    pointsCurrent[indexPoint].y = pointsCurrent[indexPoint].y + points[indexPoint].y;
                }
            }
        }
    }

    return volumes;
}

}
}
