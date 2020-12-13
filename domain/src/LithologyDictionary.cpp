#include "LithologyDictionary.h"

#include "Lithology.h"
#include <QVector>

namespace syntheticSeismic {
namespace domain {

const QVector<Lithology> &LithologyDictionary::lithologies() const
{
    return m_lithologies;
}

const Lithology& LithologyDictionary::lithology(const int lithologyId) const
{
    for(const Lithology& litho : m_lithologies){
        if(litho.getId() == lithologyId){
            return litho;
        }
    }

    return m_invalidLithology;
}

bool LithologyDictionary::changeData(const int lithologyId, const Lithology& newLithologyData)
{
    const Lithology& existingLithology = this->lithology(lithologyId);
    if (existingLithology.getId() == -1) {
        return false;
    }

    for (int i = 0, size= m_lithologies.size(); i <size; ++i) {
        const Lithology& litho = m_lithologies[i];
        if(litho.getId() == lithologyId) {
            m_lithologies[i] = Lithology(lithologyId, newLithologyData.getName(), newLithologyData.getVelocity(), newLithologyData.getDensity());
            return true;
        }
    }

    return false;
}

const Lithology& LithologyDictionary::lithology(const QString& lithologyName) const
{
    for(const Lithology& litho : m_lithologies){
        if(litho.getName() == lithologyName){
            return litho;
        }
    }

    return m_invalidLithology;
}

int LithologyDictionary::addLithology(const int id, const QString& name, const double velocity, const double density)
{
    if (id < 0 || name.isEmpty()) {
        return -1;
    }

    for(const Lithology& litho : m_lithologies){
        if(litho.getId() == id || litho.getName() == name){
            return -1;
        }
    }

    m_lithologies.push_back(Lithology(id, name, velocity, density));
    return m_lithologies.size() - 1;
}

}
}
