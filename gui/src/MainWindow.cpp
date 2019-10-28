#include "MainWindow.h"

#include "ui_MainWindow.h"
#include "ui_SplashScreen.h"

#include "GuiConstants.h"


namespace Ui {
class MainWindow;
}


namespace invertseis {
namespace gui {

class MainWindowPrivate
{
public:
    explicit MainWindowPrivate(MainWindow* q);

    void createDefaultMenus();

public:
    Q_DECLARE_PUBLIC(MainWindow)
    MainWindow* q_ptr;
    Ui::MainWindow m_ui;
    QVector<QMenu*> m_menus;
};

MainWindowPrivate::MainWindowPrivate(MainWindow* q)
    : q_ptr(q)
    , m_ui()
    , m_menus()
{
}

void MainWindowPrivate::createDefaultMenus()
{
    Q_Q(MainWindow);
    m_menus.push_back(q->menuBar()->addMenu(MENU_FILE));
}

/***************************************************************/

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , d_ptr(new MainWindowPrivate(this))
{
    d_ptr->m_ui.setupUi(this);
    Ui::SplashScreen ssui;
    QWidget* widget = new QWidget(this);
    ssui.setupUi(widget);
    setCentralWidget(widget);

    d_ptr->createDefaultMenus();
}

QVector<QMenu *> MainWindow::menus() const
{
    Q_D(const MainWindow);
    return d->m_menus;
}

QMenu* MainWindow::menu(const QString &menuTitle) const
{
    Q_D(const MainWindow);

    for(QMenu* menu : d->m_menus){
        if(menu->title() == menuTitle){
            return menu;
        }
    }

    return nullptr;
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

}
}
