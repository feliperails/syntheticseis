#include "Lithology.h"

namespace invertseis {
namespace domain {

Lithology::Lithology(const int code)
    : m_lithologyId(code)
{
}

Lithology::Lithology()
    : m_lithologyId(-1)
{
}

int Lithology::id() const
{
    return m_lithologyId;
}

}
}
