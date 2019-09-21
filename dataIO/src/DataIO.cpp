#include "DataIO.h"
#include <QPointer>

namespace invertseis {
namespace dataIO {

namespace {
static QPointer<DataIO> s_instance = nullptr;
}

DataIO::DataIO()
{
    Q_ASSERT_X(s_instance.isNull(), "DataIO::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    s_instance = QPointer<DataIO>(this);
}

void DataIO::init()
{
}

DataIO& DataIO::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "DataIO::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    return *s_instance;
}

}
}
