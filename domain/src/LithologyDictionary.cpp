#include "LithologyDictionary.h"

#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {

LithologyDictionary::LithologyDictionary(syntheticSeismic::data::EntityManager& entityManager)
    : m_entityManager(entityManager)
{    
}

QLinkedList<data::Entity*> LithologyDictionary::lithologyEntities() const
{
    return m_entityManager.entities<Lithology>();
}

data::Entity* LithologyDictionary::lithologyEntity(const int lithologyId) const
{
    auto list = m_entityManager.entities<Lithology>();
    for(data::Entity* entity : list){
        if(entity->domainObject<Lithology>()->id() == lithologyId){
            return entity;
        }
    }

    return nullptr;
}

data::Entity* LithologyDictionary::lithologyEntity(const QString& lithologyName) const
{
    auto list = m_entityManager.entities<Lithology>();
    for(data::Entity* entity : list){
        if(entity->name() == lithologyName){
            return entity;
        }
    }

    return nullptr;
}

}
}
