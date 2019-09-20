#ifndef INC_INVERTSEIS_ENTITY_H
#define INC_INVERTSEIS_ENTITY_H

#include "DomainObject.h"
#include "Metaclass.h"

#include <memory>
#include <QObject>

namespace invertseis {
namespace core {

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT

public:
    Entity(std::shared_ptr<DomainObject> domainObject, const QString& name);
    virtual ~Entity();

    std::shared_ptr<DomainObject> domainObject() const;
    DomainObject& mutableDomainObject();

    Metaclass domainType() const;

    void setName(const QString& name);
    QString name() const;

private:
    void dataAdded();
    void dataAboutToBeModified();
    void dataModified();
    void dataAboutToRemoved();
    void dataRemoved();

    void entityChanged();
    void entityAboutToBeRemoved();
    void entityAboutToChange();

private:
    Q_DECLARE_PRIVATE(Entity)
    EntityPrivate* const d_ptr;
};

} // namespace core
} // namespace invertseis
#endif
