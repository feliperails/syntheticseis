#pragma once

#include "RegularGrid.h"
#include "Wavelet.h"
#include <memory>

namespace syntheticSeismic {
namespace domain {

class ConvolutionRegularGridCalculator
{
public:
    ConvolutionRegularGridCalculator();

    std::shared_ptr<RegularGrid<double>> execute(RegularGrid<double> &impedanceRegularGrid, Wavelet &wavelet);
};

} // namespace domain
} // namespace syntheticSeismic
