#include "RickerWaveletCalculator.h"
#include "Wavelet.h"
#include <cmath>

namespace syntheticSeismic {
namespace domain {

RickerWaveletCalculator::RickerWaveletCalculator()
    : m_step(4.0)
    , m_frequency(30.0)
    , m_wlength(120)
    , m_units("ms")
{

}

std::shared_ptr<Wavelet> RickerWaveletCalculator::extract() const
{
    auto wavelet = std::make_shared<Wavelet>();
    wavelet->setFrequency(m_frequency);
    wavelet->setStep(m_step);
    // wavelet->setFirst()
    // wavelet->setLast()
    wavelet->setName(QString("Ricker wavelet (") + QString::number(wavelet->getFrequency()) + ")");
    wavelet->setUnits(m_units);

    const int halfLength = std::round(0.5 * m_wlength / m_step);
    const double wl2 = halfLength * m_step;
    wavelet->setFirst(-wl2);
    wavelet->setLast(wl2);

    size_t betaSize = halfLength * 2 + 1;
    std::vector<double> beta(betaSize);
    std::vector<double> &traces = wavelet->getTraces();
    size_t indexTrace = 0;
    double maximum = -std::numeric_limits<double>::max();
    for (int position = -halfLength; position <= halfLength; ++position)
    {
        const double value = position * m_step * m_frequency * M_PI * 0.001;
        beta[indexTrace] = value * value;
        traces[indexTrace] = (1 - beta[indexTrace]) * 2.0 * std::exp(-beta[indexTrace]);
        if (maximum > traces[indexTrace])
        {
            maximum = traces[indexTrace];
        }
        ++indexTrace;
    }

    for (size_t indexTrace = 0; indexTrace < betaSize; ++indexTrace)
    {
        traces[indexTrace] = traces[indexTrace] / maximum;
    }

    return wavelet;
}

} // namespace domain
} // namespace syntheticSeismic
