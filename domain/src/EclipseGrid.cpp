#include "EclipseGrid.h"

namespace syntheticSeismic {
namespace domain {

EclipseGrid::EclipseGrid()
    : m_numberOfCellsInX(0)
    , m_numberOfCellsInY(0)
    , m_numberOfCellsInZ(0)
    , m_coordinates()
    , m_zValues()
    , m_lithologyIds()
    , m_ages()
    , m_actnums()
{
}

EclipseGrid::EclipseGrid(const size_t numberOfCellsInX,
                         const size_t numberOfCellsInY,
                         const size_t numberOfCellsInZ,
                         const std::vector<geometry::Coordinate> &coordinates,
                         const std::vector<double> &zCoordinates,
                         const std::vector<int>& lithologyIds,
                         const std::vector<int>& faciesAssociationIds,
                         const std::vector<double>& ages,
                         const std::vector<bool>& actnums)
    : m_numberOfCellsInX(numberOfCellsInX)
    , m_numberOfCellsInY(numberOfCellsInY)
    , m_numberOfCellsInZ(numberOfCellsInZ)
    , m_coordinates(coordinates)
    , m_zValues(zCoordinates)
    , m_lithologyIds(lithologyIds)
    , m_faciesAssociationIds(faciesAssociationIds)
    , m_ages(ages)
    , m_actnums(actnums)
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

const std::vector<syntheticSeismic::geometry::Coordinate>& EclipseGrid::coordinates() const
{
    return m_coordinates;
}

const std::vector<double> &EclipseGrid::zCoordinates() const
{
    return m_zValues;
}

const std::vector<int> &EclipseGrid::lithologyIds() const
{
    return m_lithologyIds;
}

const std::vector<double>& EclipseGrid::ages() const
{
    return m_ages;
}

const std::vector<bool>& EclipseGrid::actnums() const
{
    return m_actnums;
}

const std::vector<int> &EclipseGrid::faciesAssociationIds() const
{
    return m_faciesAssociationIds;
}

}
}
