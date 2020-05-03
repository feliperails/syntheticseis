#pragma once

#include "domain/src/EclipseGrid.h"

namespace invertseis {
namespace dataIO {

class EclipseGridReader
{
public:
    explicit EclipseGridReader(const QString& path);

    invertseis::domain::EclipseGrid read(bool *ok = nullptr) const;

    const QString& path() const;

private:
    const QString m_path;
};

} // namespace dataio
} // namespace invertseis
