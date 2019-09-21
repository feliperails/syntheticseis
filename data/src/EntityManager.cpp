#include "EntityManager.h"

#include <QSharedPointer>

namespace invertseis {
namespace data {

class EntityManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(EntityManager)
    explicit EntityManagerPrivate(EntityManager* q);

    ~EntityManagerPrivate();

    EntityManager* q_ptr;
    QSet<Entity*> m_entities;
};

EntityManagerPrivate::EntityManagerPrivate(EntityManager *q)
    : q_ptr(q)
    , m_entities()
{
}

EntityManagerPrivate::~EntityManagerPrivate()
{
    qDeleteAll(m_entities);
    m_entities = QSet<Entity*>();
}

/**********************************************************/

EntityManager::EntityManager()
    : d_ptr(new EntityManagerPrivate(this))
{
}

EntityManager::~EntityManager()
{
    delete d_ptr;
}

Entity* EntityManager::createEntity(const QUuid& uuid, const QString& name, std::shared_ptr<DomainObject> domainObject)
{
    Q_D(EntityManager);
    if(name.isEmpty() || uuid.isNull()){
        return nullptr;
    }

    Entity* e = new Entity(uuid, domainObject, name);

    d->m_entities.insert(e);

    return e;
}

Entity* EntityManager::createEntity(const QString& name, std::shared_ptr<DomainObject> domainObject)
{
    return createEntity(QUuid::createUuid(), name, domainObject);
}

Entity* EntityManager::entity(const QUuid& uuid) const
{
    if(uuid.isNull()){
        return nullptr;
    }

    Q_D(const EntityManager);
    for(Entity* entity : d->m_entities){
        if(entity->uuid() == uuid){
            return entity;
        }
    }

    return nullptr;
}

Entity* EntityManager::entity(const QString& name) const
{
    if(name.isEmpty()){
        return nullptr;
    }

    Q_D(const EntityManager);
    for(Entity* entity : d->m_entities){
        if(entity->name() == name){
            return entity;
        }
    }

    return nullptr;
}

bool EntityManager::removeEntity(Entity* entity)
{
    Q_D(EntityManager);

    if(!entity){
        return false;
    }

    if(!d->m_entities.contains(entity)){
        return false;
    }

    Q_EMIT entity->aboutToBeRemoved();
    d->m_entities.remove(entity);
    delete entity;
    return true;
}

const QSet<Entity *> &EntityManager::entities() const
{
    Q_D(const EntityManager);
    return d->m_entities;
}

}
}
