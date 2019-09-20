#include "Entity.h"

namespace invertseis {
namespace core {

class EntityPrivate
{
public:
    Q_DECLARE_PUBLIC(Entity)
    explicit EntityPrivate(Entity* q, const DomainObject& domainObject);

    Entity* q_ptr;
    std::shared_ptr<DomainObject> m_domainObject;
    QString m_name;
};

EntityPrivate::EntityPrivate(Entity *q,
                             std::shared_ptr<DomainObject> domainObject,
                             const QString& name)
    : q_ptr(q)
    , m_domainObject(std::move(domainObject))
    , m_name(name)
{
}

/**********************************************************/

Entity::Entity()
    : d_ptr(new EntityPrivate())
{
}

Entity::~Entity()
{
    delete d_ptr;
}

Entity::Entity(std::shared_ptr<DomainObject> domainObject,
               const QString& name)
    : d_ptr(new EntityPrivate(this, domainObject, name))
{
}

std::shared_ptr<DomainObject> Entity::domainObject() const
{
    Q_D(const Entity);
    return d->m_domainObject;
}

Metaclass Entity::domainType() const
{
    Q_D(const Entity);
    if(d->m_domainObject.get()){
        return d->m_domainObject->metaClass();
    }

    return Metaclass();
}

void Entity::setName(const QString& name)const
{
    Q_D(Entity);
    d->m_name = name;
}
QString Entity::name() const
{
    Q_D(const Entity);
    return d->m_name;
}

}
}
