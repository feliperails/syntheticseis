#include "EclipseGridReader.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

namespace invertseis {
namespace dataIO {

namespace {
const QLatin1Literal SECTION_END_TOKEN = QLatin1Literal("/");
const QLatin1Literal ASTERISK = QLatin1Literal("*");
}

EclipseGridReader::EclipseGridReader(const QString& path)
    : m_path(path)
{
}

invertseis::domain::EclipseGrid EclipseGridReader::read(bool *ok) const
{
    if(m_path.isEmpty()){
        return invertseis::domain::EclipseGrid();
    }

    QFile file(m_path);
    if(!file.open(QFile::ReadOnly)){
        return invertseis::domain::EclipseGrid();
    }

    size_t numberOfCellsInX = 0;
    size_t numberOfCellsInY = 0;
    size_t numberOfCellsInZ = 0;

    QVector<invertseis::geometry::Coordinate> coordinates;
    QVector<double> zValues;
    QVector<int> lithologyIds;

    QTextStream stream(&file);

    bool convertionOk = true;
    QString line;
    while(!stream.atEnd()){
        line = stream.readLine();
        if(line.startsWith(QLatin1String("SPECGRID"))){
            line = stream.readLine();
            while(line.isEmpty()){
                line = stream.readLine();
            }

            if(line == SECTION_END_TOKEN){
                if(ok){ *ok = false; }
                return {};
            }

            const QStringList list = line.split(" ");
            if (list.size() != 3){
                return {};
            }

            numberOfCellsInX = static_cast<size_t>(list[0].toInt(&convertionOk));
            if(!convertionOk){
                if(ok){ *ok = false; }
                return {};
            }

            numberOfCellsInY = static_cast<size_t>(list[1].toInt(&convertionOk));
            if(!convertionOk){
                if(ok){ *ok = false; }
                return {};
            }

            numberOfCellsInZ = static_cast<size_t>(list[2].toInt(&convertionOk));
            if(!convertionOk){
                if(ok){ *ok = false; }
                return {};
            }
        } else if(line.startsWith(QLatin1String("COORD"))){
            line = stream.readLine();
            QStringList list;
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
            while(line != SECTION_END_TOKEN){
                list = line.split(QLatin1Literal(" "));
                if(line.isEmpty()){
                    continue;
                }

                if(list.size() != 3){
                    if(ok){ *ok = false; }
                    return {};
                }

                QString str = list.at(0);
                x = str.toDouble(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                y = str.toDouble(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                z = str.toDouble(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                coordinates.push_back(invertseis::geometry::Coordinate(x, y, z));
            }
        } else if(line.startsWith(QLatin1String("ZCORN"))){
            line = stream.readLine();

            QStringList splittedData;
            double z = 0.0;
            int quantity = 0;
            while(line != SECTION_END_TOKEN){
                splittedData = line.split(ASTERISK);
                if(splittedData.size() == 2){
                   z = splittedData.at(0).toDouble(&convertionOk);
                   if(!convertionOk){
                       if(ok){ *ok = false; }
                       return {};
                   }
                   quantity = splittedData.at(1).toInt(&convertionOk);
                   if(!convertionOk){
                       if(ok){ *ok = false; }
                       return {};
                   }

                   zValues.append(QVector<double>(quantity, z));
                }else{
                    z = line.toDouble(&convertionOk);
                    if(!convertionOk){
                        if(ok){ *ok = false; }
                        return {};
                    }
                    zValues.push_back(z);
                }
            }
        }
        else if (line.startsWith(QLatin1String("LITHOLOGYTYPE"))){
            line = stream.readLine();
            int id = -1;
            while(line != SECTION_END_TOKEN){
                id = line.toInt(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                lithologyIds.push_back(id);
            }
        }
    }


    if(ok){
        *ok = true;
    }

    return invertseis::domain::EclipseGrid(numberOfCellsInX,
                                           numberOfCellsInY,
                                           numberOfCellsInZ,
                                           coordinates,
                                           zValues,
                                           lithologyIds);
}

QString EclipseGridReader::path() const
{
    return m_path;
}

}
}
