#ifndef INC_INVERTSEIS_DOMAIN_ECLIPSEGRID_H
#define INC_INVERTSEIS_DOMAIN_ECLIPSEGRID_H

#include <geometry/src/Coordinate.h>

#include <data/src/DomainObject.h>

#include <QVector>

namespace invertseis {
namespace domain {

class EclipseGrid : public data::DomainObject
{
public:
    EclipseGrid();

    EclipseGrid(const EclipseGrid&) = default;
    EclipseGrid& operator =(const EclipseGrid& o);

    EclipseGrid(const size_t numberOfCellsInX,
                const size_t numberOfCellsInY,
                const size_t numberOfCellsInZ,
                const QVector<invertseis::geometry::Coordinate>& coordinates,
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
    QVector<invertseis::geometry::Coordinate> m_coordinates;
    QVector<double> m_zValues;
    QVector<int> m_lithologyIds;
};

} // namespace domain
} // namespace invertseis
#endif
