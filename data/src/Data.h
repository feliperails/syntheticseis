#ifndef INC_INVERTSEIS_DATA_DATA_H
#define INC_INVERTSEIS_DATA_DATA_H

#include <QObject>

#include "EntityManager.h"

namespace invertseis {
namespace data {

class Data : public QObject
{
    Q_OBJECT
public:
    Data();

    static Data& instance();
    static EntityManager& entityManager();

private:
    std::unique_ptr<EntityManager> m_entityManager;
};

} // namespace data
} // namespace invertseis
#endif
