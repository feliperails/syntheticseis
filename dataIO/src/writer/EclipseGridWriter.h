#ifndef INC_INVERTSEIS_DATAIO_ECLIPSEGRIDWRITER_H
#define INC_INVERTSEIS_DATAIO_ECLIPSEGRIDWRITER_H

#include <domain/src/EclipseGrid.h>

namespace invertseis {
namespace dataIO {

class EclipseGridWriter
{
public:
    explicit EclipseGridWriter(const QString& path);

    bool write(const invertseis::domain::EclipseGrid& eclipseGrid) const;

    QString path() const;

private:
    const QString m_path;
};

} // namespace dataio
} // namespace invertseis
#endif


