#pragma once

#include <vector>

namespace syntheticSeismic {
namespace domain {

template <class T>
class RegularGrid
{
public:
    RegularGrid(
            size_t numberOfCellsInX, size_t numberOfCellsInY, size_t numberOfCellsInZ,
            double cellSizeInX, double cellSizeInY, double cellSizeInZ,
            T dummy
        ) : m_data(std::vector<std::vector<std::vector<T>>>(
                       numberOfCellsInX,
                       std::vector<std::vector<T>>(
                           numberOfCellsInY,
                           std::vector<T>(numberOfCellsInZ, dummy)
                       )
                   )),
            m_cellSizeInX(cellSizeInX),
            m_cellSizeInY(cellSizeInY),
            m_cellSizeInZ(cellSizeInZ),
            m_numberOfCellsInX(numberOfCellsInX),
            m_numberOfCellsInY(numberOfCellsInY),
            m_numberOfCellsInZ(numberOfCellsInZ)
    {

    }
    std::vector<std::vector<std::vector<T>>>& getData()
    {
        return m_data;
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

private:
    std::vector<std::vector<std::vector<T>>> m_data;
    double m_cellSizeInX = 0.0;
    double m_cellSizeInY = 0.0;
    double m_cellSizeInZ = 0.0;
    size_t m_numberOfCellsInX = 0;
    size_t m_numberOfCellsInY = 0;
    size_t m_numberOfCellsInZ = 0;
};

} // namespace domain
} // namespace syntheticSeismic
