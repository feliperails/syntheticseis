#include "core/src/Core.h"
#include "geometry//src/Geometry.h"
#include "data/src/Data.h"
#include "storage/src/Storage.h"
#include "dataIO/src/DataIo.h"
#include "domain/src/Domain.h"
#include "projectManager/src/ProjectManager.h"
#include "widgets/src/Widgets.h"

#include "gui/src/Gui.h"
#include "gui/src/MainWindow.h"

#include <QApplication>
#include <iostream>

//void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
//{
//    QByteArray localMsg = msg.toLocal8Bit();
//    switch (type) {
//    case QtDebugMsg:
//        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtInfoMsg:
//        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtCriticalMsg:
//        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtFatalMsg:
//        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        abort();
//    }
//}

int main()
{
    int argc = 0;
    char *argv = nullptr;;

    //    qInstallMessageHandler(myMessageOutput);
    QApplication app(argc, &argv);
    using namespace invertseis;
    using namespace core;

    int ret = -1;
    try{
        invertseis::core::Core core;
        Q_UNUSED(core);

        invertseis::geometry::Geometry geometry;
        Q_UNUSED(geometry);

        invertseis::data::Data data;
        Q_UNUSED(data);

        invertseis::domain::Domain domain;
        Q_UNUSED(domain);

        invertseis::dataIO::DataIO dataIO;
        Q_UNUSED(dataIO);

        invertseis::storage::Storage storage;
        Q_UNUSED(storage);

        invertseis::gui::Gui gui;

        invertseis::projectManager::ProjectManager projectManager;
        Q_UNUSED(projectManager);

        invertseis::widgets::Widgets widgets;
        Q_UNUSED(widgets);

        gui.startApplication();
        gui.mainWindow().setWindowState(Qt::WindowMaximized);
        gui.mainWindow().show();

        ret = app.exec();
    }catch(const std::bad_alloc& ex){
        std::cerr << ex.what();
        ret = EXIT_FAILURE;
    }
    catch(...){
        std::cerr << "Erro inesperado\n";
        ret = EXIT_FAILURE;
    }

    return ret;
}
