#pragma once

#include <domain/src/EclipseGrid.h>
#include <domain/src/ExtractVolumes.h>
#include <domain/src/RegularGrid.h>
#include <geometry/src/Point2D.h>
#include <vector>

class DomainTestValues
{
public:
    static syntheticSeismic::domain::EclipseGrid eclipseGridFromSimpleGrid();

    static std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesOfFirstLayerFromSimpleGrid();

    static std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesFromSimpleGrid();

    static std::vector<syntheticSeismic::geometry::Point2D> minimumRectangleFromSimpleGrid();

    static std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> volumesFromSimpleGridRotated30Degrees();

    static std::pair<syntheticSeismic::geometry::Point2D, double> referencePointAndAngleInRadiansFromSimpleGridRotated30Degrees();

    static std::vector<std::shared_ptr<syntheticSeismic::geometry::Volume>> unrotatedVolumesFromSimpleGridRotated30Degrees();

    static syntheticSeismic::domain::RegularGrid<size_t> regularGridFromSimpleGridRotated30Degrees();
};
