#pragma once

#include "domain/src/EclipseGrid.h"

namespace syntheticSeismic {
namespace dataIO {

class EclipseGridReader
{
public:
    explicit EclipseGridReader(const QString& path);

    syntheticSeismic::domain::EclipseGrid read(bool *ok = nullptr) const;

    const QString& path() const;

private:
    const QString m_path;
};

} // namespace dataio
} // namespace syntheticSeismic
