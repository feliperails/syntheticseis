#include "Facade.h"
#include "Lithology.h"

#include <data/src/Facade.h>
#include <data/src/EntityManager.h>

#include <QPointer>

namespace invertseis {
namespace domain {

namespace {
static QPointer<Facade> s_instance = nullptr;
}

Facade::Facade()
{
    Q_ASSERT_X(s_instance.isNull(), "Domain::instance()", "Está chamada só pode ser realiza uma única vez.");
    s_instance = QPointer<Facade>(this);

    init();
}

void Facade::init()
{
    data::EntityManager& entityManager = data::Facade::entityManager();
    auto mudstone = entityManager.createEntity(QLatin1String("Mudstone"), std::make_shared<Lithology>(1));
    auto siltite = entityManager.createEntity(QLatin1String("Siltite"), std::make_shared<Lithology>(3));
    auto fineGrainedSandstone = entityManager.createEntity(QLatin1String("Fine-grained sandstone"), std::make_shared<Lithology>(7));
    auto mediumGrainedSandstone = entityManager.createEntity(QLatin1String("Medium-grained sandstone"), std::make_shared<Lithology>(9));
    auto coarseGrainedSandstone = entityManager.createEntity(QLatin1String("Coarse-grained sandstone"), std::make_shared<Lithology>(11));
    auto veryCoarseGrained = entityManager.createEntity(QLatin1String("Very coarse-grained"), std::make_shared<Lithology>(13));
    auto conglomerate = entityManager.createEntity(QLatin1String("Conglomerate"), std::make_shared<Lithology>(15));
    auto volcanic = entityManager.createEntity(QLatin1String("Volcanic"), std::make_shared<Lithology>(24));

    SeismicWaveVelocityDictionary& seismicWaveVelocityDictionary= this->seismicWaveVelocityDictionary();
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
        instance().m_lithologyDictionary = std::make_unique<LithologyDictionary>(data::Facade::entityManager());
    }

    return *(instance().m_lithologyDictionary);
}

Facade& Facade::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Domain::instance()", "Está chamada só pode ser realiza uma única vez.");
    return *s_instance;
}

}
}
