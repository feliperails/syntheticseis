#ifndef INC_INVERTSEIS_GUI_GUI_H
#define INC_INVERTSEIS_GUI_GUI_H

#include <QObject>

namespace invertseis {
namespace gui {

class MainWindow;
class Gui : public QObject
{
public:
    Gui();
    ~Gui();

    void init();

    void startApplication();

    static Gui& instance();
    static MainWindow& mainWindow();

private:
    MainWindow* m_mainWindow;
};

} // namespace gui
} // namespace invertseis
#endif
