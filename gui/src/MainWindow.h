#ifndef INC_INVERTSEIS_GUI_MAINWINDOW_H
#define INC_INVERTSEIS_GUI_MAINWINDOW_H

#include <QMainWindow>

namespace invertseis {
namespace gui {

class MainWindowPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent);
    ~MainWindow();

    QVector<QMenu*> menus() const;

    QMenu* menu(const QString& menuTitle) const;

    //void setWelcomeWidget(std::function<void(QWidget*)> invoker);

private:
    Q_DECLARE_PRIVATE(MainWindow)
    MainWindowPrivate* d_ptr;
};

} // namespace gui
} // namespace invertseis
#endif
