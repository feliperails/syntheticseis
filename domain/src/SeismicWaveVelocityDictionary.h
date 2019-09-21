#ifndef INC_INVERTSEIS_DOMAIN_SEISMICWAGEVELOCITYTDICTIONARY_H
#define INC_INVERTSEIS_DOMAIN_SEISMICWAGEVELOCITYTDICTIONARY_H

#include <data/src/Dictionary.h>
#include <data/src/Entity.h>

#include <QHash>

namespace invertseis {
namespace domain {

class Lithology;

class SeismicWaveVelocityDictionary : public data::Dictionary
{
public:
    double velocity(const data::Entity* litohologyEntity) const;

    bool setVelocity(const data::Entity* litohologyEntity, const double velocity);

    QList<const data::Entity*> lithologyEntities() const;

private:
    QHash<const data::Entity*, double> m_data;
};

} // namespace domain
} // namespace invertseis
#endif
