#ifndef INC_INVERTSEIS_DATAIO_ECLIPSEGRIDREADER_H
#define INC_INVERTSEIS_DATAIO_ECLIPSEGRIDREADER_H

#include <domain/src/EclipseGrid.h>

namespace invertseis {
namespace dataIO {

class EclipseGridReader
{
public:
    EclipseGridReader(const QString& path);

    invertseis::domain::EclipseGrid read(bool *ok = nullptr) const;

    QString path() const;

private:
    const QString m_path;
};

} // namespace dataio
} // namespace invertseis
#endif


