#include "Gui.h"

#include "MainWindow.h"

#include <QApplication>
#include <QPointer>

namespace invertseis {
namespace gui {

namespace{
static QPointer<Gui> s_instance = nullptr;
}

Gui::Gui()
    : m_mainWindow(new MainWindow(nullptr))
{
//    Q_ASSERT(s_instance.isNull(), QLatin1String("Already created"),  QLatin1String("Somente deve ser utilizado em modo aplicação"));
    s_instance = QPointer<Gui>(this);
}

Gui::~Gui()
{
    if(m_mainWindow){
        m_mainWindow->deleteLater();
    }
}

Gui& Gui::instance()
{
    return *s_instance;
}

MainWindow& Gui::mainWindow()
{
    return *s_instance->m_mainWindow;
}

void Gui::startApplication()
{
}

}
}
