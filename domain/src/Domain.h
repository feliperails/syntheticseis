#ifndef INC_INVERTSEIS_DOMAIN_H
#define INC_INVERTSEIS_DOMAIN_H

#include "SeismicWaveVelocityDictionary.h"
#include "LithologyDictionary.h"

#include <memory>

namespace invertseis {
namespace domain {

class Domain: public QObject
{
    Q_OBJECT
public:
    Domain();
    static Domain& instance();

    static SeismicWaveVelocityDictionary& seismicWaveVelocityDictionary();

    static LithologyDictionary& lithologyDictionary();

private:
    void init();

private:
    std::unique_ptr<SeismicWaveVelocityDictionary> m_seismicWaveVelocityDictionary;
    std::unique_ptr<LithologyDictionary> m_lithologyDictionary;
};

} // namespace domain
} // namespace invertseis
#endif
