#pragma once

#include <domain/src/GrdSurface.h>
#include <domain/src/RegularGrid.h>

class StorageTestValues
{
public:
    static syntheticSeismic::domain::RegularGrid<double> regularGridImpedanceFiveFiveFive();

    static syntheticSeismic::domain::GrdSurface<double> simpleGrdSurface();
private:
    StorageTestValues() = default;
};
