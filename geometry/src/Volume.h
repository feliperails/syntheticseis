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
    int idFaciesAssociation;
    double age;
    bool actnum;
    const size_t indexVolume;
    const size_t positionX;
    const size_t positionY;

    Volume(size_t indexVolume = 0, size_t positionX = 0, size_t positionY = 0)
        : indexVolume(indexVolume)
        , actnum(true)
        , idLithology(0)
        , idFaciesAssociation(0)
        , positionX(positionX)
        , positionY(positionY)
    {

    }
};

} // namespace geometry
} // namespace syntheticSeismic
