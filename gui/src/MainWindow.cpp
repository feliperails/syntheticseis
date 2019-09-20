#include "MainWindow.h"

#include "ui_MainWindow.h"
#include "ui_SplashScreen.h"

namespace invertseis {
namespace gui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    Ui::SplashScreen ssui;
    QWidget* widget = new QWidget(this);
    ssui.setupUi(widget);
    setCentralWidget(widget);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

}
}
