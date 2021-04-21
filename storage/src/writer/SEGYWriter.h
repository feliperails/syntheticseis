#pragma once

#include <QString>

namespace syntheticSeismic {
namespace storage {

class SegyWriter
{
public:
    explicit SegyWriter(const QString& path);

    bool writeByHdf5File(const QString& hdf5FilePath) const;

    const QString& getPath() const;

private:
    const QString m_path;
};

} // namespace storage
} // namespace syntheticSeismic
