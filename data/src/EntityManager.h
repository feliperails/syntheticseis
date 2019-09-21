#ifndef INC_INVERTSEIS_DATA_ENTITYMANAGER_H
#define INC_INVERTSEIS_DATA_ENTITYMANAGER_H

#include "Entity.h"

#include <QSet>
#include <QLinkedList>

namespace invertseis {
namespace data {

class EntityManagerPrivate;
class EntityManager : public QObject
{
    Q_OBJECT

public:
    EntityManager();
    ~EntityManager();

    Entity* createEntity(const QUuid& uuid, const QString& name, std::shared_ptr<DomainObject> domainObject = std::shared_ptr<DomainObject>());
    Entity* createEntity(const QString& name, std::shared_ptr<DomainObject> domainObject = std::shared_ptr<DomainObject>());

    const QSet<Entity*>& entities() const;

    Entity* entity(const QString& name) const;
    Entity* entity(const QUuid& uuid) const;

    template<class DomainType>
    QLinkedList<Entity*> entities() const
    {
        QLinkedList<Entity*> filtered;
        for(Entity* entity : this->entities()){
            if(entity && std::dynamic_pointer_cast<DomainType>(entity->domainObject())){
                filtered.push_back(entity);
            }
        }

        return filtered;
    }

    bool removeEntity(Entity* entity);



private:
    Q_DECLARE_PRIVATE(EntityManager)
    EntityManagerPrivate* const d_ptr;
};

} // namespace data
} // namespace invertseis
#endif
