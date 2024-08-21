#include <QApplication>

#include <QCommandLineParser>
#include <QFileInfo>
#include <QDebug>

#include "Wizard.h"

#include "domain/src/Facade.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    syntheticSeismic::domain::Facade facade;
    syntheticSeismic::widgets::Wizard wizard;
    wizard.setMinimumSize(800, 850);
    wizard.setWindowTitle(QLatin1Literal("SyntheticSeismic 1.0"));
    wizard.show();

    return app.exec();
}
