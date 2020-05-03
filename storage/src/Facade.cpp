#include "Facade.h"
#include <QPointer>

namespace syntheticSeismic {
namespace storage {

namespace {
static QPointer<Facade> s_instance = nullptr;
}

Facade::Facade()
{
    Q_ASSERT_X(s_instance.isNull(), "DataIO::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Facade>(this);
}

Facade& Facade::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "DataIO::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

}
}
