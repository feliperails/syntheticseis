#pragma once

#include <domain/src/EclipseGrid.h>
#include <domain/src/ExtractVolumes.h>
#include <vector>

class DomainTestValues
{
public:
    static syntheticSeismic::domain::EclipseGrid eclipseGridFromSimpleGrid();

    static std::vector<syntheticSeismic::domain::Volume> volumesOfFirstLayerFromSimpleGrid();

    static std::vector<syntheticSeismic::domain::Volume> volumesFromSimpleGrid();
};
