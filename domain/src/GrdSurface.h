#pragma once

#include <vector>

namespace syntheticSeismic {
namespace domain {

template <class T>
class GrdSurface
{
public:
    const static int NoDataValue = 99999;

    GrdSurface(size_t numberOfCellsX, size_t numberOfCellsY, T defaultValue = NoDataValue) :
        m_numberOfCellsX(numberOfCellsX),
        m_numberOfCellsY(numberOfCellsY),
        m_data(std::vector<std::vector<T>>(numberOfCellsX, std::vector<T>(numberOfCellsY, defaultValue)))
    {

    }

    std::vector<std::vector<T>>& getData()
    {
        return m_data;
    }

    T getZMin()
    {
        T minValue = m_data[0][0];

        for (const auto &item : m_data)
        {
            for (const auto &subItem : item)
            {
                if (static_cast<int>(subItem) != NoDataValue && subItem < minValue)
                {
                    minValue = subItem;
                }
            }
        }

        return minValue;
    }

    T getZMax()
    {
        T maxValue = m_data[0][0];

        for (const auto &item : m_data)
        {
            for (const auto &subItem : item)
            {
                if (static_cast<int>(subItem) != NoDataValue && subItem > maxValue)
                {
                    maxValue = subItem;
                }
            }
        }

        return maxValue;
    }

    double getXMin() const
    {
        return m_xMin;
    }

    void setXMin(double xMin)
    {
        m_xMin = xMin;
    }

    double getXMax() const
    {
        return m_xMax;
    }

    void setXMax(double xMax)
    {
        m_xMax = xMax;
    }

    double getYMin() const
    {
        return m_yMin;
    }

    void setYMin(double yMin)
    {
        m_yMin = yMin;
    }

    double getYMax() const
    {
        return m_yMax;
    }

    void setYMax(double yMax)
    {
        m_yMax = yMax;
    }

    size_t getNumberOfCellsX() const
    {
        return m_numberOfCellsX;
    }

    size_t getNumberOfCellsY() const
    {
        return m_numberOfCellsY;
    }
private:
    size_t m_numberOfCellsX = 0;
    size_t m_numberOfCellsY = 0;
    double m_xMin = 0.0;
    double m_xMax = 0.0;
    double m_yMin = 0.0;
    double m_yMax = 0.0;
    std::vector<std::vector<T>> m_data;
};

} // namespace domain
} // namespace syntheticSeismic
