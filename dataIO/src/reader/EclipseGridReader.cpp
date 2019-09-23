#include "EclipseGridReader.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

namespace invertseis {
namespace dataIO {

EclipseGridReader::EclipseGridReader(const QString& path)
    : m_path(path)
{
}

invertseis::domain::EclipseGrid EclipseGridReader::read() const
{
    if(m_path.isEmpty()){
        return invertseis::domain::EclipseGrid();
    }

    QFile file(m_path);
    if(!file.open(QFile::ReadOnly)){
        return invertseis::domain::EclipseGrid();
    }

    size_t m_numberOfCellsInX;
    size_t m_numberOfCellsInY;
    size_t m_numberOfCellsInZ;

    QVector<invertseis::geometry::Coordinate> m_coordinates;
    QVector<float> m_zValues;
    QVector<int> m_lithologyIds;

    QTextStream stream(&file);

    bool ok = true;
    QString line;
    while(!stream.atEnd()){
        line = stream.readLine();
        if(line.startsWith(QLatin1String("SPECGRID"))){
            line = stream.readLine();
            const QStringList list = line.split(" ");
            if (list.size() != 3){
                return {};
            }

            m_numberOfCellsInX = static_cast<size_t>(list[0].toInt(&ok));
            if(!ok){
                return {};
            }

            m_numberOfCellsInY = static_cast<size_t>(list[1].toInt(&ok));
            if(!ok){
                return {};
            }

            m_numberOfCellsInZ = static_cast<size_t>(list[2].toInt(&ok));
            if(!ok){
                return {};
            }
        } else if(line.startsWith("COORD")){
//            coords = ReadSection(gridfile)
//            # These are the unique xs and ys
//            # skip by 3 because the top and bottom grids are doubled.
//            xcoords = coords[0::3]
//            xcoords = list(OrderedDict.fromkeys(xcoords))
//            ycoords = coords[1::3]
//            ycoords = list(OrderedDict.fromkeys(ycoords))
        }

    }


    return invertseis::domain::EclipseGrid(0,
                                           0,
                                           0,
                                           QVector<invertseis::geometry::Coordinate>(),
                                           QVector<float>(),
                                           QVector<int>());
}

QString EclipseGridReader::path() const
{
    return m_path;
}

}
}
