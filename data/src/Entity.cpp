#include "Entity.h"

namespace invertseis {
namespace data {

class EntityPrivate
{
public:
    Q_DECLARE_PUBLIC(Entity)
    explicit EntityPrivate(Entity* q,
                           const QUuid& uuid,
                           std::shared_ptr<DomainObject> domainObject,
                           const QString& name);

    Entity* q_ptr;
    std::shared_ptr<DomainObject> m_domainObject;
    QString m_name;
    QUuid m_uuid;
};

EntityPrivate::EntityPrivate(Entity *q,
                             const QUuid& uuid,
                             std::shared_ptr<DomainObject> domainObject,
                             const QString& name)
    : q_ptr(q)
    , m_domainObject(std::move(domainObject))
    , m_name(name)
    , m_uuid(uuid)
{
}

/**********************************************************/

Entity::Entity(const QUuid &uuid, std::shared_ptr<DomainObject> domainObject, const QString &name)
    : d_ptr(new EntityPrivate(this, uuid, domainObject, name))
{
}

Entity::~Entity()
{
    delete d_ptr;
}

std::shared_ptr<DomainObject> Entity::domainObject() const
{
    Q_D(const Entity);
    return d->m_domainObject;
}

void Entity::setDomainObject(std::shared_ptr<DomainObject> domainObject)
{
    Q_D(Entity);
    Q_EMIT domainDataAboutToBeModified();
    d->m_domainObject = domainObject;
    Q_EMIT domainDataModified();
}

void Entity::setName(const QString& name)
{
    Q_D(Entity);
    d->m_name = name;
    Q_EMIT nameChanged();
}

QString Entity::name() const
{
    Q_D(const Entity);
    return d->m_name;
}

QUuid Entity::uuid() const
{
    Q_D(const Entity);
    return d->m_uuid;
}

}
}
