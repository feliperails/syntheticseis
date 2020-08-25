#pragma once

#include <memory>
#include <QString>
#include <vector>

namespace syntheticSeismic {
namespace domain {
class Wavelet;

class RickerWaveletCalculator
{
public:
    RickerWaveletCalculator();

    std::shared_ptr<Wavelet> extract() const;
private:
    unsigned int m_step;
    unsigned int m_length;
    double m_frequency;
    QString m_units;
};

} // namespace domain
} // namespace syntheticSeismic
