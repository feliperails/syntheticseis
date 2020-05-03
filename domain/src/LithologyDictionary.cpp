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
        if(litho.id() == lithologyId){
            return litho;
        }
    }

    return m_invalidLithology;
}

const Lithology& LithologyDictionary::lithology(const QString& lithologyName) const
{
    for(const Lithology& litho : m_lithologies){
        if(litho.name() == lithologyName){
            return litho;
        }
    }

    return m_invalidLithology;
}

int LithologyDictionary::addLithology(const int id, const QString& name)
{
    if (id < 0 || name.isEmpty()) {
        return -1;
    }

    for(const Lithology& litho : m_lithologies){
        if(litho.id() == id || litho.name() == name){
            return -1;
        }
    }

    m_lithologies.push_back(Lithology(id, name));
    return m_lithologies.size() - 1;
}

}
}
