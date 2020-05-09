#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {

Lithology::Lithology(const int code, const QString &name)
    : m_lithologyId(code)
    , m_name(name)
{
    Q_ASSERT(m_lithologyId != -1);
    Q_ASSERT(!name.isEmpty());
}

Lithology::Lithology()
    : m_lithologyId(-1)
{
}

int Lithology::id() const
{
    return m_lithologyId;
}

const QString &Lithology::name() const
{
    return m_name;
}

}
}
