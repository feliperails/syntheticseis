#ifndef INC_INVERTSEIS_GUI_MAINWINDOW_H
#define INC_INVERTSEIS_GUI_MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace invertseis {
namespace gui {

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent);
    ~MainWindow();

    //void setWelcomeWidget(std::function<void(QWidget*)> invoker);

private:
    Ui::MainWindow* m_ui;
};

} // namespace gui
} // namespace invertseis
#endif
