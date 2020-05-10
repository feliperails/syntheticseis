#pragma once

#include "domain/src/EclipseGrid.h"

namespace syntheticSeismic {
namespace storage {

class EclipseGridReader
{
public:
    explicit EclipseGridReader(const QString& path);

    syntheticSeismic::domain::EclipseGrid read(QString &error) const;

    const QString& path() const;

private:
    const QString m_path;
};

} // namespace storage
} // namespace syntheticSeismic

