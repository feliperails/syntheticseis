#ifndef INC_INVERTSEIS_WIDGETS_ECLIPSEGRIDVIEW_H
#define INC_INVERTSEIS_WIDGETS_ECLIPSEGRIDVIEW_H

#include <QGraphicsView>

#include <data/src/Entity.h>

namespace invertseis {
namespace widgets {

class EclipseGridViewPrivate;
class EclipseGridView : public QWidget
{
    Q_OBJECT
public:
    EclipseGridView(QWidget* parent = nullptr);

    QPointer<data::Entity> eclipseGridEntity() const;
    bool setEclipseGridEntity(QPointer<data::Entity> eclipseGridEntity);

private:
    Q_DECLARE_PRIVATE(EclipseGridView)
    EclipseGridViewPrivate *d_ptr;
};

} // namespace widgets
} // namespace invertseis
#endif
