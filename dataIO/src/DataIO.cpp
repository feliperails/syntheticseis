#include "DataIO.h"
#include <QPointer>

namespace invertseis {
namespace dataIO {

namespace {
static QPointer<DataIO> s_instance = nullptr;
}

DataIO::DataIO()
{
    Q_ASSERT_X(s_instance.isNull(), "DataIO::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<DataIO>(this);
}

DataIO& DataIO::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "DataIO::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

}
}
