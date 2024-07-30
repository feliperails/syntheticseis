#pragma once

#include <vector>
#include <array>
#include "geometry/src/Point2D.h"
#include "geometry/src/EnumUnit.h"

namespace syntheticSeismic {
namespace domain {

template <class T>
class RegularGrid
{
public:
    RegularGrid(
            const size_t numberOfCellsInX, const size_t numberOfCellsInY, const size_t numberOfCellsInZ,
            const double cellSizeInX, const double cellSizeInY, const double cellSizeInZ,
            const EnumUnit unitInX, const EnumUnit unitInY, const EnumUnit unitInZ,
            const std::array<geometry::Point2D, 4> &rectanglePoints,
            const double zBottom,
            const double zTop,
            T defaultValue,
            const int noDataValue = 0
        ) : m_data(std::vector<std::vector<std::vector<T>>>(
                       numberOfCellsInX,
                       std::vector<std::vector<T>>(
                           numberOfCellsInY,
                           std::vector<T>(numberOfCellsInZ, defaultValue)
                       )
                   )),
            m_cellSizeInX(cellSizeInX),
            m_cellSizeInY(cellSizeInY),
            m_cellSizeInZ(cellSizeInZ),
            m_unitInX(unitInX),
            m_unitInY(unitInY),
            m_unitInZ(unitInZ),
            m_numberOfCellsInX(numberOfCellsInX),
            m_numberOfCellsInY(numberOfCellsInY),
            m_numberOfCellsInZ(numberOfCellsInZ),
            m_rectanglePoints(rectanglePoints),
            m_zBottom(zBottom),
            m_zTop(zTop),
            m_noDataValue(noDataValue)
    {

    }

    std::vector<std::vector<std::vector<T>>>& getData()
    {
        return m_data;
    }

    inline const T& getData(size_t x, size_t y, size_t z) const
    {
        return m_data[x][y][z];
    }

    inline double getCellSizeInX() const
    {
        return m_cellSizeInX;
    }

    inline double getCellSizeInY() const
    {
        return m_cellSizeInY;
    }

    inline double getCellSizeInZ() const
    {
        return m_cellSizeInZ;
    }

    inline size_t getNumberOfCellsInX() const
    {
        return m_numberOfCellsInX;
    }

    inline size_t getNumberOfCellsInY() const
    {
        return m_numberOfCellsInY;
    }

    inline size_t getNumberOfCellsInZ() const
    {
        return m_numberOfCellsInZ;
    }

    inline void setData(const std::vector<std::vector<std::vector<T>>> data)
    {
        m_data = data;
    }

    inline int getNoDataValue() const
    {
        return m_noDataValue;
    }

    std::array<geometry::Point2D, 4> getRectanglePoints() const
    {
        return m_rectanglePoints;
    }

    inline double getZBottom() const
    {
        return m_zBottom;
    }

    inline double getZTop() const
    {
        return m_zTop;
    }

    inline EnumUnit getUnitInX() const {
        return m_unitInX;
    }

    inline EnumUnit getUnitInY() const {
        return m_unitInY;
    }

    inline EnumUnit getUnitInZ() const {
        return m_unitInZ;
    }
private:
    std::vector<std::vector<std::vector<T>>> m_data;
    const double m_cellSizeInX;
    const double m_cellSizeInY;
    const double m_cellSizeInZ;
    const size_t m_numberOfCellsInX;
    const size_t m_numberOfCellsInY;
    const size_t m_numberOfCellsInZ;
    const EnumUnit m_unitInX;
    const EnumUnit m_unitInY;
    const EnumUnit m_unitInZ;
    const std::array<geometry::Point2D, 4> m_rectanglePoints;
    const double m_zBottom;
    const double m_zTop;
    const int m_noDataValue;
};

} // namespace domain
} // namespace syntheticSeismic
