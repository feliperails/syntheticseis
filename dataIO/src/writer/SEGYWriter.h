#ifndef INC_syntheticSeismic_DATAIO_ECLIPSEGRIDREADER_H
#define INC_syntheticSeismic_DATAIO_ECLIPSEGRIDREADER_H

#include <domain/src/EclipseGrid.h>

namespace syntheticSeismic {
namespace dataIO {

class EclipseGridReader
{
public:
    EclipseGridReader(const QString& path);

    syntheticSeismic::domain::EclipseGrid read(bool *ok = nullptr) const;

    QString path() const;

private:
    const QString m_path;
};

} // namespace dataio
} // namespace syntheticSeismic
#endif


