#pragma once

#include "../../../domain/src/GrdSurface.h"
#include <QFile>
#include <QString>
#include <QTextStream>

namespace syntheticSeismic {
namespace storage {

template<class T>
class GrdSurfaceWriter
{
public:
    GrdSurfaceWriter(const QString &path) : m_path(path)
    {

    }

    void write(domain::GrdSurface<T> &surface) const
    {
        QFile file(m_path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            throw std::exception("Não foi possível gravar o arquivo de superfície de idade!");
        }

        QTextStream out(&file);
        out << "DSAA\n";
        out << QString::number(surface.getNumberOfCellsX()) << " " << QString::number(surface.getNumberOfCellsY()) << "\n";
        out << QString::number(surface.getXMin(), 'f', 4) << " " << QString::number(surface.getXMax(), 'f', 4) << "\n";
        out << QString::number(surface.getYMin(), 'f', 4) << " " << QString::number(surface.getYMax(), 'f', 4) << "\n";
        out << QString::number(surface.getZMin(), 'f', 4) << " " << QString::number(surface.getZMax(), 'f', 4) << "\n";

        const auto &data = surface.getData();
        for (const auto &item : data)
        {
            for (const auto &subItem : item)
            {
                out << QString::number(subItem, 'f', 4) << " ";
            }
            out << "\n";
        }

        file.close();

        if (!qFuzzyCompare(surface.getAngle(), 0.0))
        {
            const auto length = m_path.length();
            QString pathRotate = m_path;

            if (pathRotate.mid(length - 4) == ".grd")
            {
                pathRotate = pathRotate.mid(0, length - 4) + "_info.json";
            }
            else
            {
                pathRotate += "_info.json";
            }

            QFile fileRotate(pathRotate);
            if (!fileRotate.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                throw std::exception("Não foi possível gravar o arquivo de superfície de idade!");
            }

            QTextStream outRotate(&fileRotate);
            outRotate << "{\"rotate\":"
                << QString::number(surface.getAngle(), 'f', 10) << ","
                << "\"referencePoint\":{"
                << "\"x\":" << QString::number(surface.getReferencePoint().x, 'f', 10) << ","
                << "\"y\":" << QString::number(surface.getReferencePoint().y, 'f', 10) << ","
                << "\"z\":" << QString::number(surface.getReferencePoint().z, 'f', 10)
                << "}}";

            fileRotate.close();
        }
    }
private:
    const QString m_path;
};

} // namespace storage
} // namespace syntheticSeismic
