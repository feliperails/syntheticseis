#pragma once

#include <domain/src/RegularGrid.h>
#include <geometry/src/Volume.h>
#include <memory>

class ConvertRegularGridTestValues
{
public:
    static syntheticSeismic::domain::RegularGrid<std::shared_ptr<syntheticSeismic::geometry::Volume>> regularGridLithologyOneDimensionalDepth();

    static std::vector<int> lithologiesInTime();
private:
    // Totally static class
    ConvertRegularGridTestValues() = default;
};
