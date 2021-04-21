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
    lithologyDictionary.addLithology(1, QLatin1String("Mudstone"), 2800.0, 1.0);
    lithologyDictionary.addLithology(3, QLatin1String("Siltite"), 3000.0, 1.0);
    lithologyDictionary.addLithology(5, QLatin1String("Fine-grained sandstone - arn mf"), 3200.0, 1.0);
    lithologyDictionary.addLithology(7, QLatin1String("Fine-grained sandstone"), 3200.0, 1.0);
    lithologyDictionary.addLithology(9, QLatin1String("Medium-grained sandstone"), 3500.0, 1.0);
    lithologyDictionary.addLithology(11, QLatin1String("Coarse-grained sandstone"), 3700.0, 1.0);
    lithologyDictionary.addLithology(13, QLatin1String("Very coarse-grained"), 4000.0, 1.0);
    lithologyDictionary.addLithology(15, QLatin1String("Conglomerate"), 4500.0, 1.0);
    lithologyDictionary.addLithology(17, QLatin1String("cgl sx"), 4500.0, 1.0);
    lithologyDictionary.addLithology(19, QLatin1String("bl"), 4500.0, 1.0);
    lithologyDictionary.addLithology(24, QLatin1String("Volcanic"), 6000.0, 1.0);
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
