#include "Storage.h"

#include <QPointer>

namespace invertseis {
namespace storage {

namespace {
static QPointer<Storage> s_instance = nullptr;
}

Storage::Storage()
{
    Q_ASSERT_X(s_instance.isNull(), "Storage::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    s_instance = QPointer<Storage>(this);
}

void Storage::init()
{
}

Storage& Storage::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Storage::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    return *s_instance;
}

}
}
