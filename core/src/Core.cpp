#include "Core.h"

#include <QPointer>

namespace invertseis {
namespace core {

namespace {
static QPointer<Core> s_instance = nullptr;
}

Core::Core()
{
    Q_ASSERT_X(s_instance.isNull(), "Core::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Core>(this);
}

Core& Core::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Core::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

}
}
