#ifndef INC_INVERTSEIS_ENTITY_H
#define INC_INVERTSEIS_ENTITY_H

#include "DomainObject.h"
#include "Metaclass.h"

#include <memory>
#include <QObject>
#include <QUuid>

namespace invertseis {
namespace core {

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT

public:
    Entity(const QUuid& uuid, std::shared_ptr<DomainObject> domainObject, const QString& name);
    virtual ~Entity();

    std::shared_ptr<DomainObject> domainObject() const;
    void setDomainObject(std::shared_ptr<DomainObject>);

    void setName(const QString& name);
    QString name() const;

    QUuid uuid() const;

private:
    void domainDataAboutToBeModified();
    void domainDataModified();
    void domainDataAboutToRemoved();
    void domainDataRemoved();

    void nameChanged();

private:
    Q_DECLARE_PRIVATE(Entity)
    EntityPrivate* const d_ptr;
};

} // namespace core
} // namespace invertseis
#endif
