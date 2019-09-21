#include "EclipseGridReader.h"

namespace invertseis {
namespace dataIO {

EclipseGridReader::EclipseGridReader(const QString& path)
    : m_path(path)
{
}

invertseis::domain::EclipseGrid EclipseGridReader::read() const
{
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
