#include "Facade.h"
#include "Lithology.h"

#include <QPointer>

namespace syntheticSeismic {
namespace domain {

namespace {
static QPointer<Facade> s_instance = nullptr;
}

Facade::Facade()
{
    Q_ASSERT_X(s_instance.isNull(), "syntheticSeismic::domain::Facade::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Facade>(this);

    init();
}

void Facade::init()
{
    LithologyDictionary& lithologyDictionary = this->lithologyDictionary();
    const int mudstone = lithologyDictionary.addLithology(1, QLatin1String("Mudstone"));
    const int siltite = lithologyDictionary.addLithology(3, QLatin1String("Siltite"));
    const int fineGrainedSandstone = lithologyDictionary.addLithology(7, QLatin1String("Fine-grained sandstone"));
    const int mediumGrainedSandstone = lithologyDictionary.addLithology(9, QLatin1String("Medium-grained sandstone"));
    const int coarseGrainedSandstone = lithologyDictionary.addLithology(11, QLatin1String("Coarse-grained sandstone"));
    const int veryCoarseGrained = lithologyDictionary.addLithology(13, QLatin1String("Very coarse-grained"));
    const int conglomerate = lithologyDictionary.addLithology(15, QLatin1String("Conglomerate"));
    const int volcanic = lithologyDictionary.addLithology(24, QLatin1String("Volcanic"));

    SeismicWaveVelocityDictionary& seismicWaveVelocityDictionary = this->seismicWaveVelocityDictionary();
    seismicWaveVelocityDictionary.setVelocity(mudstone, 2800.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(siltite, 3000.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(fineGrainedSandstone, 3200.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(mediumGrainedSandstone, 3500.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(coarseGrainedSandstone, 3700.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(veryCoarseGrained, 4000.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(conglomerate, 4500.0); // m/s
    seismicWaveVelocityDictionary.setVelocity(volcanic, 6000.0); // m/s
}

SeismicWaveVelocityDictionary& Facade::seismicWaveVelocityDictionary()
{
    if(!instance().m_seismicWaveVelocityDictionary){
        instance().m_seismicWaveVelocityDictionary = std::make_unique<SeismicWaveVelocityDictionary>();
    }

    return *(instance().m_seismicWaveVelocityDictionary);
}

LithologyDictionary& Facade::lithologyDictionary()
{
    if(!instance().m_lithologyDictionary){
        instance().m_lithologyDictionary = std::make_unique<LithologyDictionary>();
    }

    return *(instance().m_lithologyDictionary);
}

Facade& Facade::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "syntheticSeismic::domain::Facade::instance()", "O construtor dessa classe nao foi invocado.");
    return *s_instance;
}

}
}
