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
    wizard.show();

    return app.exec();
}
