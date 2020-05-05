#include "ExtractVolumes.h"
#include "Point2D.h"
#include <omp.h>

using namespace std;

namespace syntheticSeismic {
namespace domain {

vector<Volume> ExtractVolumes::extractFirstLayerFrom(const syntheticSeismic::domain::EclipseGrid& eclipseGrid)
{
    const size_t volumeCount = eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY();

    vector<Volume> volumes(volumeCount);

    size_t amountOfVerticalLines = eclipseGrid.numberOfCellsInY();
    size_t amountOfHorizontalLines = eclipseGrid.numberOfCellsInX();

    #pragma omp parallel for
    for (int verticalLineIndex = 0; verticalLineIndex < static_cast<int>(amountOfVerticalLines); ++verticalLineIndex)
    {
        for (size_t horizontalLineIndex = 0; horizontalLineIndex < amountOfHorizontalLines; ++horizontalLineIndex)
        {
            size_t volumeLinearIndex = horizontalLineIndex + static_cast<size_t>(verticalLineIndex) * amountOfHorizontalLines;
            // Pontos da primeira superficie horizontal do volume
            for (size_t pointIndexSurface = 0; pointIndexSurface <= 3; ++pointIndexSurface)
            {
                size_t pointIndex = pointIndexSurface;
                size_t eclipseGridCoordPointIndex = static_cast<size_t>(verticalLineIndex) * ((amountOfHorizontalLines + 1) * 2)
                        + horizontalLineIndex * 2;
                auto indexCoord = eclipseGridCoordPointIndex + pointIndexSurface;
                auto &coordinate = eclipseGrid.coordinates().at(indexCoord);
                volumes[volumeLinearIndex].m_points[pointIndex].x = coordinate.x();
                volumes[volumeLinearIndex].m_points[pointIndex].y = coordinate.y();
                volumes[volumeLinearIndex].m_points[pointIndex].z = coordinate.z();
            }
            // Pontos da segunda superficie horizontal do volume
            for (size_t pointIndexSurface = 0; pointIndexSurface <= 3; ++pointIndexSurface)
            {
                size_t pointIndex = 4 + pointIndexSurface;
                size_t eclipseGridCoordPointIndex = (static_cast<size_t>(verticalLineIndex) + 1) * ((amountOfHorizontalLines + 1) * 2)
                        + (horizontalLineIndex) * 2;

                auto indexCoord = eclipseGridCoordPointIndex + pointIndexSurface;
                auto &coordinate = eclipseGrid.coordinates().at(indexCoord);
                volumes[volumeLinearIndex].m_points[pointIndex].x = coordinate.x();
                volumes[volumeLinearIndex].m_points[pointIndex].y = coordinate.y();
                volumes[volumeLinearIndex].m_points[pointIndex].z = coordinate.z();
            }
        }
    }

    return volumes;
}

std::vector<Volume> ExtractVolumes::extractFromVolumesOfFirstLayer(const std::vector<Volume> &volumesOfFirstLayer,
                                                                   const syntheticSeismic::domain::EclipseGrid& eclipseGrid)
{
    vector<vector<Point2D>> coordinateDifferenceXYCurrent(eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY());
    for (auto &points : coordinateDifferenceXYCurrent)
    {
        points.resize(4);
    }

    vector<Volume> volumes(eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY() * eclipseGrid.numberOfCellsInZ());
    size_t indexVolume = 0;
    size_t indexZCorn = 1;
    for (int specGrid3 = 0; specGrid3 < static_cast<int>(eclipseGrid.numberOfCellsInZ()); ++specGrid3)
    {
        size_t indexVolumeStartFrontBack = indexVolume;
        for (size_t frontBack = 0; frontBack <= 1; ++frontBack)
        {
            indexVolume = indexVolumeStartFrontBack;
            size_t kStartFrontBack = frontBack * 4;
            size_t indexVolumeLine = 0;
            for (size_t specGrid2 = 0; specGrid2 < eclipseGrid.numberOfCellsInY(); ++specGrid2)
            {
                for (size_t kStart = 0; kStart <= 2; kStart += 2)
                {
                    for (size_t indexVolumeCurrent = 0; indexVolumeCurrent < eclipseGrid.numberOfCellsInX(); ++indexVolumeCurrent)
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
                indexVolumeLine = indexVolumeLine + eclipseGrid.numberOfCellsInX();
                indexVolume = indexVolume + eclipseGrid.numberOfCellsInX();
            }
        }
    }

    return volumes;
}

}
}
