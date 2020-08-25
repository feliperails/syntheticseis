#include "RickerWaveletCalculator.h"
#include "Wavelet.h"

namespace syntheticSeismic {
namespace domain {

RickerWaveletCalculator::RickerWaveletCalculator()
    : m_step(4.0)
    , m_frequency(30.0)
    , m_units("ms")
{

}

std::shared_ptr<Wavelet> RickerWaveletCalculator::extract() const
{
    auto wavelet = std::make_shared<Wavelet>();
    wavelet->setFrequency(m_frequency);
    wavelet->setStep(m_step);
    // wavelet->setFirst()
    wavelet->setName(QString("Ricker wavelet (") + QString::number(wavelet->getFrequency()) + ")");

    return wavelet;
}

} // namespace domain
} // namespace syntheticSeismic
