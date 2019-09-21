#ifndef INC_INVERTSEIS_DOMAIN_LITHOLOGYDICTIONARY_H
#define INC_INVERTSEIS_DOMAIN_LITHOLOGYDICTIONARY_H

#include <data/src/Dictionary.h>
#include <data/src/EntityManager.h>

#include <QLinkedList>

namespace invertseis {
namespace domain {

class Lithology;

class LithologyDictionary : public data::Dictionary
{
public:
    LithologyDictionary(data::EntityManager &entityManager);

    QLinkedList<data::Entity *> lithologyEntities() const;

    data::Entity* lithologyEntity(const int lithologyId) const;
    data::Entity* lithologyEntity(const QString& lithologyName) const;

private:
    invertseis::data::EntityManager& m_entityManager;
};

} // namespace domain
} // namespace invertseis
#endif
