#include <QApplication>

#include <QCommandLineParser>
#include <QFileInfo>
#include <QDebug>

#include "Wizard.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    syntheticSeismic::widgets::Wizard wizard;
    wizard.show();

    return app.exec();
}
