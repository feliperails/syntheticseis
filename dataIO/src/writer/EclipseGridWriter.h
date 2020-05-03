#pragma once
#include <domain/src/EclipseGrid.h>

namespace syntheticSeismic {
namespace dataIO {

class EclipseGridWriter
{
public:
    explicit EclipseGridWriter(const QString& path);

    bool write(const syntheticSeismic::domain::EclipseGrid& eclipseGrid) const;

    QString path() const;

private:
    const QString m_path;
};

} // namespace dataio
} // namespace syntheticSeismic
