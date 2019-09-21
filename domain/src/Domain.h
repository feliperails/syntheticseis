#ifndef INC_INVERTSEIS_DOMAIN_H
#define INC_INVERTSEIS_DOMAIN_H

#include "SeismicWaveVelocityDictionary.h"

#include <memory>

namespace invertseis {
namespace domain {

class Domain: public QObject
{
    Q_OBJECT
public:
    Domain();
    void init();
    static Domain& instance();

    static SeismicWaveVelocityDictionary& seismicWaveVelocityDictionary();

private:
    std::unique_ptr<SeismicWaveVelocityDictionary> m_seismicWaveVelocityDictionary;
};

} // namespace domain
} // namespace invertseis
#endif
