#pragma once

#include "data/src/DomainObject.h"

#include <vector>

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
