#pragma once

#include "geometry/src/Coordinate.h"

#include <QMetaType>
#include <QVector>
#include <QtGlobal>

#include <memory>

namespace syntheticSeismic {
namespace domain {

class EclipseGrid
{
public:
    EclipseGrid();

    EclipseGrid(const EclipseGrid&) = default;
    EclipseGrid& operator =(const EclipseGrid& o) = default;

    EclipseGrid(const size_t numberOfCellsInX,
                const size_t numberOfCellsInY,
                const size_t numberOfCellsInZ,
                const std::vector<syntheticSeismic::geometry::Coordinate>& coordinates,
                const std::vector<double>& zCoordinates,
                const std::vector<int>& lithologyIds);

    size_t numberOfCellsInX() const;
    size_t numberOfCellsInY() const;
    size_t numberOfCellsInZ() const;

    const std::vector<geometry::Coordinate> &coordinates() const;

    const std::vector<double>& zCoordinates() const;

    const std::vector<int>& lithologyIds() const;

private:
    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
    std::vector<syntheticSeismic::geometry::Coordinate> m_coordinates;
    std::vector<double> m_zValues;
    std::vector<int> m_lithologyIds;
};

} // namespace domain
} // namespace syntheticSeismic

Q_DECLARE_METATYPE(syntheticSeismic::domain::EclipseGrid)
Q_DECLARE_METATYPE(std::shared_ptr<syntheticSeismic::domain::EclipseGrid>)
Q_DECLARE_METATYPE(QVector<std::shared_ptr<syntheticSeismic::domain::EclipseGrid>>)
