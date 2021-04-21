#include "SegyWriter.h"
#include "infrastructure/src/OperatingSystemCommand.h"

#include <QtWidgets/QApplication>

using namespace syntheticSeismic::infrastructure;

namespace syntheticSeismic {
namespace storage {
SegyWriter::SegyWriter(const QString &path) : m_path(path)
{

}

bool SegyWriter::writeByHdf5File(const QString& hdf5FilePath) const
{
    const QString pythonPath = "python";
    const QString basePath = QApplication::applicationDirPath();
    const QString scriptPython = basePath + "/python/generate_segy.py";
    QStringList args;
    args << scriptPython;
    args << hdf5FilePath;
    args << m_path;

    OperatingSystemCommand command;
    OperatingSystemReturnCommand result = command.runCommand(pythonPath, args);

    return result.status;
}

} // namespace storage
}// namespace syntheticSeismic
