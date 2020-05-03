#pragma once

#include "data/src/DomainObject.h"

#include <vector>

namespace syntheticSeismic {
namespace domain {

class SeismicTraces : public syntheticSeismic::data::DomainObject
{
public:
    typedef std::vector<double> Trace;


private:
    std::vector<Trace> m_traces;
};

} // namespace domain
} // namespace syntheticSeismic
