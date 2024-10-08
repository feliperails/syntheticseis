#include "EclipseGridWriter.h"

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
const QLatin1Literal LITHOLOGY = QLatin1String("LITHOLOGYTYPE");
const QLatin1Literal COORD = QLatin1String("COORD");
const QLatin1Literal ZCORN = QLatin1String("ZCORN");
const QLatin1Literal SINGLE_SPACE_SEPARATOR = QLatin1Literal(" ");
}

EclipseGridWriter::EclipseGridWriter(const QString& path)
    : m_path(path)
{
}

bool EclipseGridWriter::write(const syntheticSeismic::domain::EclipseGrid& eclipseGrid) const
{
    if(m_path.isEmpty()){
        return false;
    }

    QFile file(m_path);
    if(!file.open(QFile::WriteOnly)){
        return false;
    }

    const size_t numberOfCellsInX = eclipseGrid.numberOfCellsInX();
    const size_t numberOfCellsInY = eclipseGrid.numberOfCellsInY();
    const size_t numberOfCellsInZ = eclipseGrid.numberOfCellsInZ();

    const vector<syntheticSeismic::geometry::Coordinate>& coordinates = eclipseGrid.coordinates();
    const vector<double>& zCoordinates = eclipseGrid.zCoordinates();
    const vector<int>& lithologyIds = eclipseGrid.lithologyIds();

    QTextStream stream(&file);

    // Especifições do grid.
    const int NUMBER_OF_PROPERTIES = 1; // Representa a litologia.
    const QLatin1String UNKNOWN_CHARACTER = QLatin1String("F");
    const QLatin1String NEW_LINE = QLatin1String("\n");
    stream << SPEC_GRID;
    stream << NEW_LINE;
    stream << numberOfCellsInX << SINGLE_SPACE_SEPARATOR;
    stream << numberOfCellsInY << SINGLE_SPACE_SEPARATOR;
    stream << numberOfCellsInZ << SINGLE_SPACE_SEPARATOR;
    stream << NUMBER_OF_PROPERTIES << SINGLE_SPACE_SEPARATOR;
    stream << UNKNOWN_CHARACTER << SINGLE_SPACE_SEPARATOR;
    stream << NEW_LINE;
    stream << SECTION_END_TOKEN;
    stream << NEW_LINE;

    // Coordenadas.
    stream << COORD;
    stream << NEW_LINE;
    QString currentSeparator = SINGLE_SPACE_SEPARATOR;
    for(size_t i = 0, size = coordinates.size(); i < size; ++i){
        const syntheticSeismic::geometry::Coordinate& coord =  coordinates[i];
        stream << coord.x() << currentSeparator;
        stream << coord.y() << currentSeparator;
        stream << coord.z();
        stream << NEW_LINE;
        if(i + 1 == size){
            stream << SECTION_END_TOKEN;
        }
    }
    stream << NEW_LINE;

    stream << ZCORN;
    stream << NEW_LINE;

    const int PRECISION = 4;
    for(size_t i = 0, size = zCoordinates.size(); i < size; ++i){
        stream << QString::number(zCoordinates[i], 'f', PRECISION);
        stream << NEW_LINE;
        if( i + 1 == size){
            stream << SECTION_END_TOKEN;
        }
    }

    stream << NEW_LINE;
    stream << LITHOLOGY;
    stream << NEW_LINE;
    for(size_t i = 0, size = lithologyIds.size(); i < size; ++i){
        stream << lithologyIds[i] << SINGLE_SPACE_SEPARATOR;
    }
    stream << NEW_LINE;
    stream << SECTION_END_TOKEN;
    stream.flush();

    file.close();

    return true;
}

QString EclipseGridWriter::path() const
{
    return m_path;
}

}
}
