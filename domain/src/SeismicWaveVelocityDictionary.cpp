#include "SeismicWaveVelocityDictionary.h"

#include "Lithology.h"

namespace syntheticSeismic {
namespace domain {

double SeismicWaveVelocityDictionary::velocity(const int lithologyID) const
{
    return m_data.value(lithologyID, 0.0);
}

bool SeismicWaveVelocityDictionary::setVelocity(const int lithologyID, const double velocity)
{
    if(lithologyID < 0){
        return false;
    }

    m_data.insert(lithologyID, velocity);
    return true;
}

QList<int> SeismicWaveVelocityDictionary::lithologies() const
{
    return m_data.keys();
}

}
}
