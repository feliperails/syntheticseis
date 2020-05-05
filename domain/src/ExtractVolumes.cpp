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

}
}
