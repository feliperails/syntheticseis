#pragma once

#include "../../../domain/src/GrdSurface.h"
#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QTextStream>

namespace syntheticSeismic {
namespace storage {

template<class T>
class GrdSurfaceReader
{
public:
    GrdSurfaceReader(const QString &path) : m_path(path)
    {

    }

    domain::GrdSurface<T> read() const
    {
        const QRegularExpression regex("-?[0-9.,]+");
        QFile fs(m_path);
        if (!fs.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw std::invalid_argument("Error openning " + m_path.toStdString() + "!");
        }

        // Skip DSAA
        fs.readLine();
        QList<QByteArray> dimensionParts = fs.readLine().split(' ');
        QList<QByteArray> xParts = fs.readLine().split(' ');
        QList<QByteArray> yParts = fs.readLine().split(' ');
        // Skip z-min and z-max
        fs.readLine();

        size_t numberOfCellsX = dimensionParts[0].toULongLong();
        size_t numberOfCellsY = dimensionParts[1].toULongLong();

        domain::GrdSurface<T> surface(numberOfCellsX, numberOfCellsY);
        surface.setXMin(xParts[0].toDouble());
        surface.setXMax(xParts[1].toDouble());
        surface.setYMin(yParts[0].toDouble());
        surface.setYMax(yParts[1].toDouble());

        QRegularExpression regexLine("\\s$");

        auto &data = surface.getData();
        for (size_t i = 0; i < numberOfCellsX; ++i)
        {
            const auto line = QString::fromStdString(fs.readLine().toStdString()).replace(regexLine, "").split(' ');
            for (size_t j = 0; j < numberOfCellsY; ++j)
            {
                data[i][j] = static_cast<T>(line[static_cast<int>(j)].toDouble());

            }
        }

        return surface;
    }
private:
    const QString m_path;
};

} // namespace storage
} // namespace syntheticSeismic
