#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {

Lithology::Lithology(const int& code, const QString &name, const double& velocity, const double& density, const FillingType & fillingType)
    : m_id(code)
    , m_name(name)
    , m_velocity(velocity)
    , m_density(density)
    , m_fillingType(fillingType)
{
    Q_ASSERT(m_id != -1);
    Q_ASSERT(!name.isEmpty());
}

Lithology::Lithology()
    : m_id(-1)
    , m_name("")
    , m_velocity(0.0)
    , m_density(0.0)
    , m_fillingType(FillingType::None)
{
}

Lithology::Lithology(const Lithology& _other)
    : m_id(_other.m_id)
    , m_name(_other.m_name)
    , m_velocity(_other.m_velocity)
    , m_density(_other.m_density)
    , m_fillingType(_other.m_fillingType)
{

}

const int& Lithology::getId() const
{
    return m_id;
}

void Lithology::setId(const int& id)
{
    m_id = id;
}

const QString &Lithology::getName() const
{
    return m_name;
}

void Lithology::setName(const QString& name)
{
    m_name = name;
}

const double& Lithology::getVelocity() const
{
    return m_velocity;
}

void Lithology::setVelocity(const double& velocity)
{
    m_velocity = velocity;
}

const double& Lithology::getDensity() const
{
    return m_density;
}

void Lithology::setDensity(const double& density)
{
    m_density = density;
}

const FillingType& Lithology::getFillingType() const
{
    return m_fillingType;
}

void Lithology::setFillingType(const FillingType& fillingType)
{
    m_fillingType = fillingType;
}

void Lithology::coyDataFrom(const Lithology& from)
{
    m_density = from.m_density;
    m_name = from.m_name;
    m_velocity = from.m_velocity;
    m_density = from.m_density;
    m_fillingType = from.m_fillingType;
}

}
}
