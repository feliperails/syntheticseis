#ifndef INC_INVERTSEIS_DATA_ENTITYMANAGER_H
#define INC_INVERTSEIS_DATA_ENTITYMANAGER_H

#include "Entity.h"

#include <QSet>

namespace invertseis {
namespace data {

class EntityManagerPrivate;
class EntityManager : public QObject
{
    Q_OBJECT

public:
    EntityManager();
    ~EntityManager();

    Entity* createEntity(const QUuid& uuid, const QString& name);
    const QSet<Entity*>& entities() const;

    bool removeEntity(Entity* entity);

private:
    Q_DECLARE_PRIVATE(EntityManager)
    EntityManagerPrivate* const d_ptr;
};

} // namespace data
} // namespace invertseis
#endif
