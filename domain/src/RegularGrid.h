#pragma once

#include <vector>
#include <array>
#include "geometry/src/Point2D.h"

namespace syntheticSeismic {
namespace domain {

template <class T>
class RegularGrid
{
public:
    RegularGrid(
            size_t numberOfCellsInX, size_t numberOfCellsInY, size_t numberOfCellsInZ,
            double cellSizeInX, double cellSizeInY, double cellSizeInZ,
            T defaultValue,
            int noDataValue = 0
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
            m_numberOfCellsInX(numberOfCellsInX),
            m_numberOfCellsInY(numberOfCellsInY),
            m_numberOfCellsInZ(numberOfCellsInZ),
            m_noDataValue(noDataValue)
    {

    }

    std::vector<std::vector<std::vector<T>>>& getData()
    {
        return m_data;
    }

    const T& getData(size_t x, size_t y, size_t z) const
    {
        return m_data[x][y][z];
    }

    double getCellSizeInX() const
    {
        return m_cellSizeInX;
    }

    double getCellSizeInY() const
    {
        return m_cellSizeInY;
    }

    double getCellSizeInZ() const
    {
        return m_cellSizeInZ;
    }

    size_t getNumberOfCellsInX() const
    {
        return m_numberOfCellsInX;
    }

    size_t getNumberOfCellsInY() const
    {
        return m_numberOfCellsInY;
    }

    size_t getNumberOfCellsInZ() const
    {
        return m_numberOfCellsInZ;
    }

    void setData(const std::vector<std::vector<std::vector<T>>> data)
    {
        m_data = data;
    }

    int getNoDataValue() const
    {
        return m_noDataValue;
    }

    void setNoDataValue(int value)
    {
        m_noDataValue = value;
    }

    std::array<geometry::Point2D, 4> getRectanglePoints() const
    {
        return m_rectanglePoints;
    }

    void setRectanglePoints(const std::array<geometry::Point2D, 4> &value)
    {
        m_rectanglePoints = value;
    }

private:
    std::vector<std::vector<std::vector<T>>> m_data;
    double m_cellSizeInX;
    double m_cellSizeInY;
    double m_cellSizeInZ;
    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;
    std::array<geometry::Point2D, 4> m_rectanglePoints;
    int m_noDataValue;
};

} // namespace domain
} // namespace syntheticSeismic
