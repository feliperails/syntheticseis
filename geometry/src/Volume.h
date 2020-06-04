#pragma once

#include "Point3D.h"
#include <array>
#include <limits>

namespace syntheticSeismic {
namespace geometry {

class Volume
{
public:
    static const int UNDEFINED_LITHOLOGY = -1;

    std::array<geometry::Point3D, 8> points;
    int idLithology;
    size_t indexVolume;
    Volume(size_t indexVolume = 0)
    {
        this->idLithology = 0;
        this->indexVolume = indexVolume;
    }
};

} // namespace geometry
} // namespace syntheticSeismic
