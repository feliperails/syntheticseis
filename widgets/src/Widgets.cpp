#include "Widgets.h"

#include "EclipseGridView.h"

#include <data/src/Data.h>
#include <data/src/EntityManager.h>

#include <gui/src/Gui.h>
#include <gui/src/GuiConstants.h>
#include <gui/src/MainWindow.h>

#include <dataIO/src/reader/EclipseGridReader.h>

#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QPointer>

namespace invertseis {
namespace widgets {

namespace {
data::Entity* importEclipseGrid(const QString& fileName)
{
    if(fileName.isEmpty()){
        return nullptr;
    }

    dataIO::EclipseGridReader reader(fileName);

    bool ok = false;
    auto eclipseGrid = std::make_shared<domain::EclipseGrid>(reader.read(&ok));
    if (!ok){
        return nullptr;
    }

    QFileInfo fileInfo(fileName);

    QString entityName = fileInfo.fileName();
    entityName.remove(QString(".%1").arg(fileInfo.suffix()));

    data::EntityManager& entityManager = data::Data::entityManager();
    return entityManager.createEntity(entityName, eclipseGrid);
}
}

Widgets::Widgets()
{
    EclipseGridView* view = new EclipseGridView(QApplication::activeWindow());

    gui::Gui::mainWindow().setCentralWidget(view);

    gui::Gui::mainWindow().menu(gui::MENU_FILE)->addAction(QObject::tr("Importar EclipseGrid"), [view](const bool){
        const QString fileName = QFileDialog::getOpenFileName(view,
                                                              QObject::tr("Eclipse Grid"),
                                                              QCoreApplication::applicationDirPath(),
                                                              QObject::tr("EclipseGrid (*.grdecl)"));

        auto entity = importEclipseGrid(fileName);
        if(entity){
            view->setEclipseGridEntity(QPointer<data::Entity>(entity));
        }
    });

    view->setEclipseGridEntity(importEclipseGrid(QCoreApplication::applicationDirPath() + "/" + "EclipseGridTest.grdecl"));
}
}
}
