#include "Core.h"

#include <QPointer>

namespace invertseis {
namespace core {

namespace {
static QPointer<Core> s_instance = nullptr;
}

Core::Core()
{
    Q_ASSERT_X(s_instance.isNull(), "Core::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    s_instance = QPointer<Core>(this);
}

void Core::init()
{
}

Core& Core::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Core::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    return *s_instance;
}

}
}
