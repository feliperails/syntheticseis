#ifndef INC_INVERTSEIS_DOMAIN_SEISMICTRACES_H
#define INC_INVERTSEIS_DOMAIN_SEISMICTRACES_H

#include <data/src/DomainObject.h>

#include <QVector>

namespace invertseis {
namespace domain {

class SeismicTraces : public invertseis::data::DomainObject
{
public:
    typedef std::vector<double> Trace;


private:
    std::vector<Trace> m_traces;
};

} // namespace domain
} // namespace invertseis
#endif
