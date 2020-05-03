#pragma once

#include "DomainObject.h"

#include <memory>
#include <QObject>
#include <QUuid>

namespace invertseis {
namespace data {

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT

public:
    Entity(const QUuid& uuid, std::shared_ptr<DomainObject> domainObject, const QString& name);
    virtual ~Entity();

    template<class DomainType>
    std::shared_ptr<DomainType> domainObject() const
    {
        return std::dynamic_pointer_cast<DomainType>(this->domainObject());
    }

    std::shared_ptr<DomainObject> domainObject() const;
    void setDomainObject(std::shared_ptr<DomainObject>);

    void setName(const QString& name);
    QString name() const;

    QUuid uuid() const;

Q_SIGNALS:
    void domainDataAboutToBeModified();
    void domainDataModified();
    void domainDataAboutToRemoved();
    void domainDataRemoved();

    void nameChanged();

    void aboutToBeRemoved();

private:
    Q_DECLARE_PRIVATE(Entity)
    EntityPrivate* const d_ptr;
};

} // namespace data
} // namespace invertseis
