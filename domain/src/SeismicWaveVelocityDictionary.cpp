#include "SeismicWaveVelocityDictionary.h"

#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {

double SeismicWaveVelocityDictionary::velocity(const data::Entity *lithology) const
{
    return m_data.value(lithology, 0.0);
}

bool SeismicWaveVelocityDictionary::setVelocity(const data::Entity *lithologyEntity, const double velocity)
{
    if(!lithologyEntity){
        return false;
    }

    if(!lithologyEntity->domainObject()){
        return false;
    }

    if(!std::dynamic_pointer_cast<Lithology>(lithologyEntity->domainObject())){
        return false;
    }

    m_data.insert(lithologyEntity, velocity);
    return true;
}

QList<const data::Entity*> SeismicWaveVelocityDictionary::lithologyEntities() const
{
    return m_data.keys();
}

}
}
