#include "EclipseGridReader.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <iostream>

namespace invertseis {
namespace dataIO {

namespace {
const QLatin1Literal SECTION_END_TOKEN = QLatin1Literal("/");
const QLatin1Literal ASTERISK = QLatin1Literal("*");
const QLatin1Literal SPEC_GRID = QLatin1String("SPECGRID");
const QLatin1Literal LITHOLOGY_TYPE = QLatin1String("LITHOLOGYTYPE");
const QLatin1Literal COORD = QLatin1String("COORD");
const QLatin1Literal ZCORN = QLatin1String("ZCORN");
const QLatin1Literal SINGLE_SPACE_SEPARATOR = QLatin1Literal(" ");
}

EclipseGridReader::EclipseGridReader(const QString& path)
    : m_path(path)
{
}

invertseis::domain::EclipseGrid EclipseGridReader::read(bool *ok) const
{
    if(m_path.isEmpty()){
        if(ok){ *ok = false; }
        return invertseis::domain::EclipseGrid();
    }

    QFile file(m_path);
    if(!file.open(QFile::ReadOnly)){
        if(ok){ *ok = false; }
        return invertseis::domain::EclipseGrid();
    }

    size_t numberOfCellsInX = 0;
    size_t numberOfCellsInY = 0;
    size_t numberOfCellsInZ = 0;

    QVector<invertseis::geometry::Coordinate> coordinates;
    QVector<double> zValues;
    QVector<int> lithologyIds;

    QTextStream stream(&file);
    int row = 0;

    bool convertionOk = true;
    QString line;
    while(!stream.atEnd()){
        line = stream.readLine();
        ++row;
        if(line.startsWith(SPEC_GRID)){
            line = stream.readLine();
            ++row;
            while(line.isEmpty()){
                line = stream.readLine();
                ++row;
            }

            if(line == SECTION_END_TOKEN){
                if(ok){ *ok = false; }
                return {};
            }

            line.replace(SECTION_END_TOKEN, QString());
            line = line.simplified();
            const QStringList list = line.split(" ");
            if (list.size() < 3){
                if(ok){ *ok = false; }
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
        } else if(line.startsWith(COORD)){
            line = stream.readLine();
            ++row;
            QStringList list;
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
            bool sectionEnded  = false;

            while(line != SECTION_END_TOKEN){

                sectionEnded = line.contains(SECTION_END_TOKEN);
                if(sectionEnded){
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                list = line.split(SINGLE_SPACE_SEPARATOR);

                if(line.isEmpty()){
                    line = stream.readLine();
                    ++row;
                    if(sectionEnded){
                        line = SECTION_END_TOKEN;
                    }
                    continue;
                }

                if(list.size() != 3){
                    if(ok){ *ok = false; }
                    return {};
                }

                x = list.at(0).toDouble(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                y = list.at(1).toDouble(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                z = list.at(2).toDouble(&convertionOk);
                if(!convertionOk){
                    if(ok){ *ok = false; }
                    return {};
                }

                coordinates.push_back(invertseis::geometry::Coordinate(x, y, z));
                if(!sectionEnded){
                    line = stream.readLine();
                    ++row;
                }else{
                    line = SECTION_END_TOKEN;
                }
            }
        } else if(line.startsWith(ZCORN)){
            line = stream.readLine();
            ++row;

            QStringList splittedData;
            double z = 0.0;
            int quantity = 0;
            bool sectionEnded = false;
            while(line != SECTION_END_TOKEN){

                sectionEnded = line.contains(SECTION_END_TOKEN);
                if(sectionEnded){
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();

                splittedData = line.split(ASTERISK);
                if(splittedData.size() == 2){
                    quantity = splittedData.at(0).toInt(&convertionOk);
                    if(!convertionOk){
                        if(ok){ *ok = false; }
                        return {};
                    }

                    z = splittedData.at(1).toDouble(&convertionOk);
                    if(!convertionOk){
                        if(ok){ *ok = false; }
                        return {};
                    }

                    zValues.append(QVector<double>(quantity, z));
                }else{
                    if(!line.isEmpty()){

                        z = line.toDouble(&convertionOk);
                        if(!convertionOk){
                            if(ok){ *ok = false; }
                            return {};
                        }
                        zValues.push_back(z);
                    }
                }
                if(sectionEnded){
                    line = SECTION_END_TOKEN;
                }else{
                    line = stream.readLine();
                    ++row;
                }
            }
        }
        else if (line.startsWith(LITHOLOGY_TYPE)){
            line = stream.readLine();
            ++row;
            int id = -1;
            bool sectionEnded = false;
            QStringList splitedData;
            while(line != SECTION_END_TOKEN){
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if(sectionEnded){
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                splitedData = line.split(" ");

                if(!splitedData.isEmpty()){
                    for(const QString str : splitedData){
                        id = str.toInt(&convertionOk);
                        if(!convertionOk && !str.isEmpty()){
                            if(ok){ *ok = false; }
                            return {};
                        }
                        lithologyIds.push_back(id);
                    }
                }
                else{
                    id = line.toInt(&convertionOk);
                    if(!convertionOk){
                        if(ok){ *ok = false; }
                        return {};
                    }
                }

                if(sectionEnded){
                    line = SECTION_END_TOKEN;
                }else{
                    line = stream.readLine();
                    ++row;

                }
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
