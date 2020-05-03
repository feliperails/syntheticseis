#pragma once

#include "geometry/src/Coordinate.h"

#include <QVector>

namespace syntheticSeismic {
namespace domain {

class EclipseGrid
{
public:
    EclipseGrid();

    EclipseGrid(const EclipseGrid&) = default;
    EclipseGrid& operator =(const EclipseGrid& o);

    EclipseGrid(const size_t numberOfCellsInX,
                const size_t numberOfCellsInY,
                const size_t numberOfCellsInZ,
                const QVector<syntheticSeismic::geometry::Coordinate>& coordinates,
                const QVector<double>& zCoordinates,
                const QVector<int>& lithologyIds);

    size_t numberOfCellsInX() const;
    size_t numberOfCellsInY() const;
    size_t numberOfCellsInZ() const;

    const QVector<geometry::Coordinate> &coordinates() const;

    const QVector<double>& zCoordinates() const;

    const QVector<int>& lithologyIds() const;

private:
    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
    QVector<syntheticSeismic::geometry::Coordinate> m_coordinates;
    QVector<double> m_zValues;
    QVector<int> m_lithologyIds;
};

} // namespace domain
} // namespace syntheticSeismic
