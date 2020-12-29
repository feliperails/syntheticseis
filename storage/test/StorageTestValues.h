#pragma once

#include <domain/src/RegularGrid.h>

class StorageTestValues
{
public:
    static syntheticSeismic::domain::RegularGrid<double> regularGridImpedanceFiveFiveFive();
private:
    StorageTestValues() = default;
};
