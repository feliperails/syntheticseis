#include "EclipseGrid.h"

using namespace std;

namespace syntheticSeismic {
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
                         const vector<geometry::Coordinate> &coordinates,
                         const vector<double> &zCoordinates,
                         const vector<int>& lithologyIds)
    : m_numberOfCellsInX(numberOfCellsInX)
    , m_numberOfCellsInY(numberOfCellsInY)
    , m_numberOfCellsInZ(numberOfCellsInZ)
    , m_coordinates(coordinates)
    , m_zValues(zCoordinates)
    , m_lithologyIds(lithologyIds)
{
}

//EclipseGrid& EclipseGrid::operator=(const EclipseGrid& o)
//{
//    m_numberOfCellsInX = o.m_numberOfCellsInX;
//    m_numberOfCellsInY = o.m_numberOfCellsInY;
//    m_numberOfCellsInZ = o.m_numberOfCellsInZ;

//    m_coordinates = o.m_coordinates;
//    m_lithologyIds = o.m_lithologyIds;
//    m_zValues = o.m_zValues;

//    return *this;
//}

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

const vector<syntheticSeismic::geometry::Coordinate>& EclipseGrid::coordinates() const
{
    return m_coordinates;
}

const vector<double> &EclipseGrid::zCoordinates() const
{
    return m_zValues;
}

const vector<int> &EclipseGrid::lithologyIds() const
{
    return m_lithologyIds;
}

}
}
