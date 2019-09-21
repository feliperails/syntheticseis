#include "Storage.h"

#include <QPointer>

namespace invertseis {
namespace storage {

namespace {
static QPointer<Storage> s_instance = nullptr;
}

Storage::Storage()
{
    Q_ASSERT_X(s_instance.isNull(), "Storage::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Storage>(this);
}

Storage& Storage::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Storage::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

}
}
