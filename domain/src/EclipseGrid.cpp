#include "EclipseGrid.h"

namespace invertseis {
namespace domain {

EclipseGrid::EclipseGrid()
    : m_numberOfCellsInX(0)
    , m_numberOfCellsInY(0)
    , m_numberOfCellsInZ(0)
    , m_coordinates()
    , m_zValues()
    , m_lithologyIds()
{
}

EclipseGrid::EclipseGrid(const size_t numberOfCellsInX,
                         const size_t numberOfCellsInY,
                         const size_t numberOfCellsInZ,
                         const QVector<geometry::Coordinate> &coordinates, const QVector<float> &zValues,
                         const QVector<int>& lithologyIds)
    : m_numberOfCellsInX(numberOfCellsInX)
    , m_numberOfCellsInY(numberOfCellsInY)
    , m_numberOfCellsInZ(numberOfCellsInZ)
    , m_coordinates(coordinates)
    , m_zValues(zValues)
    , m_lithologyIds(lithologyIds)
{
}

size_t EclipseGrid::numberOfCellsInX() const
{
    return m_numberOfCellsInX;
}

size_t EclipseGrid::numberOfCellsInY() const
{
    return m_numberOfCellsInY;
}

size_t EclipseGrid::numberOfCellsInZ() const
{
    return m_numberOfCellsInZ;
}

const QVector<invertseis::geometry::Coordinate>& EclipseGrid::coordinates() const
{
    return m_coordinates;
}

const QVector<float> &EclipseGrid::zValues() const
{
    return m_zValues;
}

const QVector<int> &EclipseGrid::lithologyIds() const
{
    return m_lithologyIds;
}

}
}
