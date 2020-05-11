#include "EclipseGridReader.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <iostream>

using namespace std;

namespace syntheticSeismic {
namespace storage {

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

syntheticSeismic::domain::EclipseGrid EclipseGridReader::read(QString& error) const
{
    error.clear();

    if(m_path.isEmpty()){
        error = QObject::tr("Empty path");
        return {};
    }

    QFile file(m_path);
    if(!file.open(QFile::ReadOnly)){
        error = QObject::tr("Error when reading the file: %1").arg(file.errorString());
        return {};
    }

    size_t numberOfCellsInX = 0;
    size_t numberOfCellsInY = 0;
    size_t numberOfCellsInZ = 0;

    vector<syntheticSeismic::geometry::Coordinate> coordinates;
    vector<double> zValues;
    vector<int> lithologyIds;

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
                error = QObject::tr("Premature end of section:Error when reading the file: %1");
                return {};
            }

            line.replace(SECTION_END_TOKEN, QString());
            line = line.simplified();
            const QStringList list = line.split(" ");
            if (list.size() < 3){
                error = QObject::tr("Premature end of section:Error when reading the file: %1");
                return {};
            }

            numberOfCellsInX = static_cast<size_t>(list[0].toInt(&convertionOk));
            if(!convertionOk){
                error = QObject::tr("Error when retrieving the number of cells on x");
                return {};
            }

            numberOfCellsInY = static_cast<size_t>(list[1].toInt(&convertionOk));
            if(!convertionOk){
                error = QObject::tr("Error when retrieving the number of cells on y");
                return {};
            }

            numberOfCellsInZ = static_cast<size_t>(list[2].toInt(&convertionOk));
            if(!convertionOk){
                error = QObject::tr("Error when retrieving the number of cells on z");
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
                    error = QObject::tr("It's necessary at least 3 coords: %1").arg(line);
                    return {};
                }

                x = list.at(0).toDouble(&convertionOk);
                if(!convertionOk){
                    error = QObject::tr("Error when convertion x value to double: %1").arg(line.at(0));
                    return {};
                }

                y = list.at(1).toDouble(&convertionOk);
                if(!convertionOk){
                    error = QObject::tr("Error when convertion y value to double: %1").arg(line.at(1));
                    return {};
                }

                z = list.at(2).toDouble(&convertionOk);
                if(!convertionOk){
                    error = QObject::tr("Error when convertion z value to double: %1").arg(line.at(2));
                    return {};
                }

                coordinates.push_back(syntheticSeismic::geometry::Coordinate(x, y, z));
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
                        error = QObject::tr("Error when retrieving value quantity: %1").arg(splittedData.at(0));
                        return {};
                    }

                    z = splittedData.at(1).toDouble(&convertionOk);
                    if(!convertionOk){
                        error = QObject::tr("Error when conveting z value to int: %1").arg(splittedData.at(1));
                        return {};
                    }

                    for (int addIndex = 0; addIndex < quantity; ++addIndex)
                    {
                        zValues.push_back(z);
                    }
                }else{
                    if(!line.isEmpty()){

                        z = line.toDouble(&convertionOk);
                        if(!convertionOk){
                            error = QObject::tr("Error when conveting z value to double: %1").arg(splittedData.at(1));
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
                    for(const QString& str : splitedData){
                        id = str.toInt(&convertionOk);
                        if(!convertionOk && !str.isEmpty()){
                            error = QObject::tr("Error when conveting value to int: %1").arg(str);
                            return {};
                        }
                        lithologyIds.push_back(id);
                    }
                }
                else{
                    id = line.toInt(&convertionOk);
                    if(!convertionOk){
                        error = QObject::tr("Error when conveting value to int: %1").arg(id);
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

    return syntheticSeismic::domain::EclipseGrid(numberOfCellsInX,
                                           numberOfCellsInY,
                                           numberOfCellsInZ,
                                           coordinates,
                                           zValues,
                                           lithologyIds);
}

const QString &EclipseGridReader::path() const
{
    return m_path;
}

}
}
