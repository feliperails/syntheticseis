#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {

Lithology::Lithology(const int code, const QString &name, double velocity, double density)
    : m_id(code)
    , m_name(name)
    , m_velocity(velocity)
    , m_density(density)
{
    Q_ASSERT(m_id != -1);
    Q_ASSERT(!name.isEmpty());
}

Lithology::Lithology()
    : m_id(-1)
    , m_name("")
    , m_velocity(0.0)
    , m_density(0.0)
{
}

int Lithology::getId() const
{
    return m_id;
}

const QString &Lithology::getName() const
{
    return m_name;
}

double Lithology::getVelocity() const
{
    return m_velocity;
}

double Lithology::getDensity() const
{
    return m_density;
}

}
}
