#include <QApplication>
#include <QFileInfo>
#include <QIcon>
#include <QDebug>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>

#include "Wizard.h"

#include "domain/src/Facade.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/appIco"));

    syntheticSeismic::domain::Facade facade;
    syntheticSeismic::widgets::Wizard wizard;
    wizard.setMinimumSize(850, 875);
    wizard.setWindowTitle(QLatin1Literal("SyntheticSeismic 2.0.4"));
    wizard.show();

    return app.exec();
}
