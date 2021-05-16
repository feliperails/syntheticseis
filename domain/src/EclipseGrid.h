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
    const static int NoDataValue = -99999;

    EclipseGrid();

    EclipseGrid(const EclipseGrid&) = default;
    EclipseGrid& operator =(const EclipseGrid& o) = default;

    EclipseGrid(const size_t numberOfCellsInX,
                const size_t numberOfCellsInY,
                const size_t numberOfCellsInZ,
                const std::vector<geometry::Coordinate>& coordinates,
                const std::vector<double>& zCoordinates,
                const std::vector<int>& lithologyIds,
                const std::vector<int>& faciesAssociationIds,
                const std::vector<double>& ages,
                const std::vector<bool>& actnums);

    size_t numberOfCellsInX() const;
    size_t numberOfCellsInY() const;
    size_t numberOfCellsInZ() const;

    const std::vector<geometry::Coordinate> &coordinates() const;

    const std::vector<double>& zCoordinates() const;

    const std::vector<int>& lithologyIds() const;

    const std::vector<int>& faciesAssociationIds() const;

    const std::vector<double>& ages() const;

    const std::vector<bool>& actnums() const;

private:
    const size_t m_numberOfCellsInX;
    const size_t m_numberOfCellsInY;
    const size_t m_numberOfCellsInZ;
    const std::vector<geometry::Coordinate> m_coordinates;
    const std::vector<double> m_zValues;
    const std::vector<int> m_lithologyIds;
    const std::vector<int> m_faciesAssociationIds;
    const std::vector<double> m_ages;
    const std::vector<bool> m_actnums;
};

} // namespace domain
} // namespace syntheticSeismic

Q_DECLARE_METATYPE(syntheticSeismic::domain::EclipseGrid)
Q_DECLARE_METATYPE(std::shared_ptr<syntheticSeismic::domain::EclipseGrid>)
Q_DECLARE_METATYPE(QVector<std::shared_ptr<syntheticSeismic::domain::EclipseGrid>>)
