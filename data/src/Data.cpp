#include "Data.h"

#include <QPointer>

namespace invertseis {
namespace data {

namespace {
static QPointer<Data> s_instance = nullptr;
}

Data::Data()
    : m_entityManager(std::make_unique<EntityManager>())
{
    Q_ASSERT_X(s_instance.isNull(), "Data::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Data>(this);
}

Data& Data::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Data::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

EntityManager& Data::entityManager()
{
    return *(instance().m_entityManager);
}

}
}
