#pragma once

#include <QObject>

#include "EntityManager.h"

namespace syntheticSeismic {
namespace data {

class Facade : public QObject
{
    Q_OBJECT
public:
    Facade();

    static Facade& instance();
    static EntityManager& entityManager();

private:
    std::unique_ptr<EntityManager> m_entityManager;
};

} // namespace data
} // namespace syntheticSeismic
