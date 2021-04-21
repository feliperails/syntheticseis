#include "OperatingSystemCommand.h"
#include "OperatingSystemReturnCommand.h"
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>
#include <QDebug>

namespace syntheticSeismic {
namespace infrastructure {

OperatingSystemReturnCommand OperatingSystemCommand::runCommand(const QString program, const QStringList args)
{
    OperatingSystemReturnCommand returnProcess;
    returnProcess.status = false;

    // Code block for monitoring via terminal when used via debug
	// When the executed process presents an error
    QProcess* process = new QProcess();
    connect(process, &QProcess::errorOccurred, this, [=] {
        qDebug() << "----------------------------------";
        qDebug() << " process " << program << " error" ;
        qDebug() << process->errorString();
        qDebug() << process->arguments();
        qDebug() << process->workingDirectory();
        qDebug() << process->processId();
        qDebug() << "----------------------------------";
    });

    // Code block for monitoring via terminal when used via debug
	// When the executed process is complete
    connect(process, static_cast<void(QProcess::*) (int, QProcess::ExitStatus)>(&QProcess::finished),
		this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
			Q_UNUSED(exitStatus)

			qDebug() << "  -----------------";
            qDebug() << "  process " << program << " finished" << exitCode ;
			qDebug() << "  -----------------";
		});

    // Code block for monitoring via terminal when used via debug
	// When the executed process starts
    connect(process, &QProcess::started, this, [=] {
        qDebug() << "----------------------------------";
        qDebug() << "process " << program << " started";
        qDebug() << "----------------------------------";
        qDebug() << args.join(" ");
    });

    process->start(program, args);

    // Waits for the process to finish
    if (process->waitForFinished(-1))
    {
        returnProcess.status = (process->exitStatus() == QProcess::ExitStatus::NormalExit && process->exitCode() == 0);

        QString auxReadAllStandardOutput = QString::fromUtf8(process->readAllStandardOutput());
        QString auxReadAllStandardError = QString::fromUtf8(process->readAllStandardError());
        returnProcess.output = (auxReadAllStandardOutput +  auxReadAllStandardError);
    }
    else
    {
        returnProcess.output = QString("Error executing programName " + program + " via console.");
    }

    delete process;

    return returnProcess;
}

} // namespace infrastructure
} // namespace syntheticSeismic
