#include "Domain.h"

#include <QPointer>

namespace invertseis {
namespace domain {

namespace {
static QPointer<Domain> s_instance = nullptr;
}

Domain::Domain()
{
    Q_ASSERT_X(s_instance.isNull(), "Domain::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    s_instance = QPointer<Domain>(this);
}

void Domain::init()
{    
}

SeismicWaveVelocityDictionary& Domain::seismicWaveVelocityDictionary()
{
    if(!instance().m_seismicWaveVelocityDictionary){
        instance().m_seismicWaveVelocityDictionary = std::make_unique<SeismicWaveVelocityDictionary>();
    }

    return *(instance().m_seismicWaveVelocityDictionary);
}

Domain& Domain::instance()
{
    Q_ASSERT_X(!s_instance.isNull(), "Domain::instance()", "Illegal call. Application global state, can not be used without a plugin or in test environment. ");
    return *s_instance;
}

}
}
