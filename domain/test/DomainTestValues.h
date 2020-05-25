#pragma once

#include <domain/src/EclipseGrid.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/Point2D.h>
#include <domain/src/RegularGrid.h>
#include <vector>

class DomainTestValues
{
public:
    static syntheticSeismic::domain::EclipseGrid eclipseGridFromSimpleGrid();

    static std::vector<syntheticSeismic::domain::Volume> volumesOfFirstLayerFromSimpleGrid();

    static std::vector<syntheticSeismic::domain::Volume> volumesFromSimpleGrid();

    static std::vector<syntheticSeismic::domain::Point2D> minimumRectangleFromSimpleGrid();

    static std::vector<syntheticSeismic::domain::Volume> volumesFromSimpleGridRotated30Degrees();

    static std::pair<syntheticSeismic::domain::Point2D, double> referencePointAndAngleInRadiansFromSimpleGridRotated30Degrees();

    static std::vector<syntheticSeismic::domain::Volume> unrotatedVolumesFromSimpleGridRotated30Degrees();

    static syntheticSeismic::domain::RegularGrid<size_t> regularGridFromSimpleGridRotated30Degrees();
};
