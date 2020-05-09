#pragma once

#include <vector>

namespace syntheticSeismic {
namespace domain {

class SeismicTraces
{
public:
    typedef std::vector<double> Trace;


private:
    std::vector<Trace> m_traces;
};

} // namespace domain
} // namespace syntheticSeismic
