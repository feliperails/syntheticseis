#include "Data.h"

#include <QPointer>

namespace invertseis {
namespace data {

namespace {
static QPointer<Data> s_instance = nullptr;
}

Data::Data()
{
    Q_ASSERT_X(s_instance.isNull(), "Data::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    s_instance = QPointer<Data>(this);
}

void Data::init()
{
}

Data& Data::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Data::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    return *s_instance;
}

}
}
