#include "Facade.h"

#include <QPointer>

namespace invertseis {
namespace data {

namespace {
static QPointer<Facade> s_instance = nullptr;
}

Facade::Facade()
    : m_entityManager(std::make_unique<EntityManager>())
{
    Q_ASSERT_X(s_instance.isNull(), "Data::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Facade>(this);
}

Facade& Facade::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Data::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

EntityManager& Facade::entityManager()
{
    return *(instance().m_entityManager);
}

}
}
