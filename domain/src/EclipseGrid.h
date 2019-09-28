#ifndef INC_INVERTSEIS_DOMAIN_H
#define INC_INVERTSEIS_DOMAIN_H

#include <geometry/src/Coordinate.h>

#include <data/src/DomainObject.h>

#include <QVector>

namespace invertseis {
namespace domain {

class EclipseGrid : public data::DomainObject
{
public:
    EclipseGrid();

    EclipseGrid(const size_t numberOfCellsInX,
                const size_t numberOfCellsInY,
                const size_t numberOfCellsInZ,
                const QVector<invertseis::geometry::Coordinate>& coordinates,
                const QVector<double>& zValues,
                const QVector<int>& lithologyIds);

    size_t numberOfCellsInX() const;
    size_t numberOfCellsInY() const;
    size_t numberOfCellsInZ() const;

    const QVector<geometry::Coordinate> &coordinates() const;

    const QVector<double>& zValues() const;

    const QVector<int>& lithologyIds() const;

private:
    const size_t m_numberOfCellsInX;
    const size_t m_numberOfCellsInY;
    const size_t m_numberOfCellsInZ;
    const QVector<invertseis::geometry::Coordinate> m_coordinates;
    const QVector<double> m_zValues;
    const QVector<int> m_lithologyIds;
};

} // namespace domain
} // namespace invertseis
#endif
