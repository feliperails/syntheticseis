#include "EclipseGridReader.h"

#include <QDebug>
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
const QLatin1Literal LITHOLOGY = QLatin1String("LITHOLOGY");
const QLatin1Literal FACIESASSOCIATION = QLatin1String("FACIESASSOCIATION");
const QLatin1Literal AGE = QLatin1String("AGE");
const QLatin1Literal ACTNUM = QLatin1String("ACTNUM");
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

    if (m_path.isEmpty())
    {
        error = QObject::tr("Empty path");
        qDebug() << error;
        return {};
    }

    QFile file(m_path);
    if (!file.open(QFile::ReadOnly))
    {
        error = QObject::tr("Error when reading the file: %1").arg(file.errorString());
        qDebug() << error;
        return {};
    }

    size_t numberOfCellsInX = 0;
    size_t numberOfCellsInY = 0;
    size_t numberOfCellsInZ = 0;

    vector<syntheticSeismic::geometry::Coordinate> coordinates;
    vector<double> zValues;
    vector<int> lithologyIds;
    vector<int> faciesAssociationIds;
    vector<double> ages;
    vector<bool> actnums;

    QTextStream stream(&file);
    int row = 0;

    bool convertionOk = true;
    QString line;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        ++row;
        if (line.startsWith(SPEC_GRID))
        {
            line = stream.readLine();
            ++row;
            while (line.isEmpty())
            {
                line = stream.readLine();
                ++row;
            }

            if (line == SECTION_END_TOKEN)
            {
                error = QObject::tr("Premature end of section:Error when reading the file: %1");
                qDebug() << error;
                return {};
            }

            line.replace(SECTION_END_TOKEN, QString());
            line = line.simplified();
            const QStringList list = line.split(" ");
            if (list.size() < 3)
            {
                error = QObject::tr("Premature end of section:Error when reading the file: %1");
                qDebug() << error;
                return {};
            }

            numberOfCellsInX = static_cast<size_t>(list[0].toInt(&convertionOk));
            if (!convertionOk)
            {
                error = QObject::tr("Error when retrieving the number of cells on x");
                qDebug() << error;
                return {};
            }

            numberOfCellsInY = static_cast<size_t>(list[1].toInt(&convertionOk));
            if (!convertionOk)
            {
                error = QObject::tr("Error when retrieving the number of cells on y");
                qDebug() << error;
                return {};
            }

            numberOfCellsInZ = static_cast<size_t>(list[2].toInt(&convertionOk));
            if (!convertionOk)
            {
                error = QObject::tr("Error when retrieving the number of cells on z");
                qDebug() << error;
                return {};
            }
        }
        else if (line.startsWith(COORD))
        {
            line = stream.readLine();
            ++row;
            bool sectionEnded = false;

            vector<double> coords;
            // If there are nx volumes in relation to the x coordinate, then they are nx + 1 points in relation to the x coordinate.
            // So "(numberOfCellsInX + 1)".
            // If ny volumes are in relation to the y coordinate, then they are ny + 1 points in relation to the y coordinate.
            // So "(numberOfCellsInY + 1)".
            // Each coordinate has 3 points.
            // So "* 3".
            // COORD stores 2 plans.
            // So "* 2".
            const size_t numberOfCoordValues = (numberOfCellsInX + 1) * (numberOfCellsInY + 1) * 3 * 2;
            coords.reserve(numberOfCoordValues);
            while (line != SECTION_END_TOKEN)
            {
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if (sectionEnded)
                {
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                QStringList list = line.split(SINGLE_SPACE_SEPARATOR);

                if (line.isEmpty())
                {
                    line = stream.readLine();
                    ++row;
                    if (sectionEnded)
                    {
                        line = SECTION_END_TOKEN;
                    }
                    continue;
                }

                for (const auto &item : list)
                {
                    const auto value = item.toDouble(&convertionOk);
                    if (!convertionOk)
                    {
                        error = QObject::tr("Error when convertion COORD value to double: %1").arg(item);
                        qDebug() << error;
                        return {};
                    }
                    coords.push_back(value);
                }
                if (!sectionEnded)
                {
                    line = stream.readLine();
                    ++row;
                }
                else
                {
                    line = SECTION_END_TOKEN;
                }
            }

            if (coords.size() != numberOfCoordValues)
            {
                error = QObject::tr("Incorrect number of COORD values. Expected %1, found %2.").arg(coords.size()).arg(numberOfCoordValues);
                qDebug() << error;
                return {};
            }

            // Calculating the number of coordinates is the same as the COORD number, but obviously without multiplying by 3.
            coordinates.reserve((numberOfCellsInX + 1) * (numberOfCellsInY + 1) * 2);

            // The values of x, y and z are stored sequentially in COORD. Therefore, point storage skips every 3 COORD.
            for (size_t i = 0; i < numberOfCoordValues; i += 3)
            {
                coordinates.push_back(geometry::Coordinate(coords[i], coords[i + 1], coords[i + 2]));
            }
        }
        else if (line.startsWith(ZCORN))
        {
            zValues.reserve(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ * 8);

            line = stream.readLine();
            ++row;

            QStringList splittedData;
            double z = 0.0;
            size_t quantity = 0;
            bool sectionEnded = false;
            while (line != SECTION_END_TOKEN)
            {
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if (sectionEnded)
                {
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                if (!line.isEmpty())
                {
                    const auto zValuesString = line.split(" ");

                    for (const auto &zValueString : zValuesString)
                    {
                        splittedData = zValueString.split(ASTERISK);
                        if (splittedData.size() == 2)
                        {
                            quantity = splittedData.at(0).toULong(&convertionOk);
                            if (!convertionOk)
                            {
                                error = QObject::tr("Error when retrieving value quantity: %1").arg(splittedData.at(0));
                                qDebug() << error;
                                return {};
                            }

                            z = splittedData.at(1).toDouble(&convertionOk);
                            if (!convertionOk)
                            {
                                error = QObject::tr("Error when conveting z value to double: %1").arg(splittedData.at(1));
                                qDebug() << error;
                                return {};
                            }

                            zValues.insert(zValues.end(), quantity, z);
                        }
                        else
                        {
                            z = zValueString.toDouble(&convertionOk);
                            if (!convertionOk)
                            {
                                error = QObject::tr("Error when conveting z value to double: %1").arg(zValueString);
                                qDebug() << error;
                                return {};
                            }
                            zValues.push_back(z);
                        }
                    }
                }
                if (sectionEnded)
                {
                    line = SECTION_END_TOKEN;
                }
                else
                {
                    line = stream.readLine();
                    ++row;
                }
            }
        }
        else if (line.startsWith(LITHOLOGY))
        {
            lithologyIds.reserve(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);

            line = stream.readLine();
            ++row;
            int id = -1;
            bool sectionEnded = false;
            QStringList splitData;
            while (line != SECTION_END_TOKEN)
            {
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if (sectionEnded)
                {
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                splitData = line.split(SINGLE_SPACE_SEPARATOR);

                if (!splitData.isEmpty())
                {
                    for (const QString& str : splitData)
                    {
                        id = str.toInt(&convertionOk);
                        if (!convertionOk && !str.isEmpty())
                        {
                            error = QObject::tr("Error when conveting value to int: %1").arg(str);
                            qDebug() << error;
                            return {};
                        }
                        lithologyIds.push_back(id);
                    }
                }
                else
                {
                    id = line.toInt(&convertionOk);
                    if (!convertionOk)
                    {
                        error = QObject::tr("Error when conveting value to int: %1").arg(line);
                        qDebug() << error;
                        return {};
                    }
                }

                if (sectionEnded)
                {
                    line = SECTION_END_TOKEN;
                }
                else
                {
                    line = stream.readLine();
                    ++row;

                }
            }
        }
        else if (line.startsWith(FACIESASSOCIATION))
        {
            faciesAssociationIds.reserve(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);

            line = stream.readLine();
            ++row;
            int id = -1;
            bool sectionEnded = false;
            QStringList splitData;
            while (line != SECTION_END_TOKEN)
            {
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if (sectionEnded)
                {
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                splitData = line.split(SINGLE_SPACE_SEPARATOR);

                if (!splitData.isEmpty())
                {
                    for (const QString& str : splitData)
                    {
                        id = str.toInt(&convertionOk);
                        if (!convertionOk && !str.isEmpty())
                        {
                            error = QObject::tr("Error when conveting value to int: %1").arg(str);
                            qDebug() << error;
                            return {};
                        }
                        faciesAssociationIds.push_back(id);
                    }
                }
                else
                {
                    id = line.toInt(&convertionOk);
                    if (!convertionOk)
                    {
                        error = QObject::tr("Error when conveting value to int: %1").arg(line);
                        qDebug() << error;
                        return {};
                    }
                }

                if (sectionEnded)
                {
                    line = SECTION_END_TOKEN;
                }
                else
                {
                    line = stream.readLine();
                    ++row;

                }
            }
        }
        else if (line.startsWith(AGE))
        {
            ages.resize(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ, domain::EclipseGrid::NoDataValue);

            line = stream.readLine();
            ++row;
            double age = -1.0;
            bool sectionEnded = false;
            size_t indexAge = 0;
            QStringList splitData;
            while (line != SECTION_END_TOKEN)
            {
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if (sectionEnded)
                {
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                splitData = line.split(SINGLE_SPACE_SEPARATOR);

                if (!splitData.isEmpty())
                {
                    for (const QString& str : splitData)
                    {
                        age = str.toDouble(&convertionOk);
                        if (!convertionOk && !str.isEmpty())
                        {
                            error = QObject::tr("Error when conveting value to double: %1").arg(str);
                            qDebug() << error;
                            return {};
                        }
                        ages[indexAge] = age;
                        ++indexAge;
                    }
                }
                else
                {
                    age = line.toDouble(&convertionOk);
                    if (!convertionOk)
                    {
                        error = QObject::tr("Error when conveting value to double: %1").arg(line);
                        qDebug() << error;
                        return {};
                    }
                }

                if (sectionEnded)
                {
                    line = SECTION_END_TOKEN;
                }
                else
                {
                    line = stream.readLine();
                    ++row;

                }
            }
        }
        else if (line.startsWith(ACTNUM))
        {
            actnums.reserve(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);

            line = stream.readLine();
            ++row;
            bool sectionEnded = false;
            while (line != SECTION_END_TOKEN)
            {
                sectionEnded = line.contains(SECTION_END_TOKEN);
                if (sectionEnded)
                {
                    line.replace(SECTION_END_TOKEN, QString());
                }

                line = line.simplified();
                QStringList splitData = line.split(SINGLE_SPACE_SEPARATOR);

                if (!splitData.isEmpty())
                {
                    for (const QString& str : splitData)
                    {
                        bool actNum = static_cast<bool>(str.toInt(&convertionOk));
                        if (!convertionOk && !str.isEmpty())
                        {
                            error = QObject::tr("Error when conveting value to int: %1").arg(str);
                            qDebug() << error;
                            return {};
                        }
                        actnums.push_back(actNum);
                    }
                }

                if (sectionEnded)
                {
                    line = SECTION_END_TOKEN;
                }
                else
                {
                    line = stream.readLine();
                    ++row;

                }
            }
        }
    }

    lithologyIds.resize(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);
    faciesAssociationIds.resize(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ);
    ages.resize(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ, domain::EclipseGrid::NoDataValue);
    actnums.resize(numberOfCellsInX * numberOfCellsInY * numberOfCellsInZ, true);

    return {
        numberOfCellsInX,
        numberOfCellsInY,
        numberOfCellsInZ,
        coordinates,
        zValues,
        lithologyIds,
        faciesAssociationIds,
        ages,
        actnums
    };
}

const QString &EclipseGridReader::path() const
{
    return m_path;
}

}
}
