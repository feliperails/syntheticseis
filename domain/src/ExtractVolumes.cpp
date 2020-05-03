#include "ExtractVolumes.h"

namespace syntheticSeismic {
namespace domain {

std::vector<Volume> ExtractVolumes::extractFirstLayerFrom(const syntheticSeismic::domain::EclipseGrid& eclipseGrid)
{
    const size_t volumeCount = eclipseGrid.numberOfCellsInX() * eclipseGrid.numberOfCellsInY();

    std::vector<Volume> volumes(volumeCount);

    const int verticalSurfaceCount = 2;

    // A camada principal é delimitada por duas superficies sobrepostos.
    //
    for(int verticalSurface = 0; verticalSurface < verticalSurfaceCount; ++verticalSurface)
    {
        // Foi multiplicado por quatro pois a primeira face do volume nao estrutura possui quatro pontos.
        const int initialPointInTheVolume = verticalSurface * 4;

        // Índice do volume atual
        int volumeIndex = 0;

        // Ponto no volume
        int indexPointInTheVolume = initialPointInTheVolume;

        // ????
        bool completeVolumePrevious = false;

        //
        for(int i = verticalSurface, size = eclipseGrid.coordinates().size(); i < size; ++i){

        }
    }

    return volumes;
}

}
}
