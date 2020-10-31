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

    double getStep() const;
    void setStep(const double step);

    double getFrequency() const;
    void setFrequency(const double frequency);

    unsigned int getLength() const;

    QString getUnits() const;

private:
    double m_step;
    unsigned int m_length;
    double m_frequency;
    unsigned int m_wlength;
    QString m_units;
};

} // namespace domain
} // namespace syntheticSeismic
