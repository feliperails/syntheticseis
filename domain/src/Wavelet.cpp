#include "Wavelet.h"

namespace syntheticSeismic {
namespace domain {

Wavelet::Wavelet()
    : m_first(0.0)
    , m_last(0.0)
    , m_step(0.0)
{

}

QString Wavelet::getName() const
{
    return m_name;
}

void Wavelet::setName(const QString &name)
{
    m_name = name;
}

double Wavelet::getFirst() const
{
    return m_first;
}

void Wavelet::setFirst(double first)
{
    m_first = first;
}

double Wavelet::getLast() const
{
    return m_last;
}

void Wavelet::setLast(double last)
{
    m_last = last;
}

double Wavelet::getStep() const
{
    return m_step;
}

void Wavelet::setStep(double step)
{
    m_step = step;
}

QString Wavelet::getUnits() const
{
    return m_units;
}

void Wavelet::setUnits(const QString &units)
{
    m_units = units;
}

std::vector<double>& Wavelet::getTraces()
{
    return m_traces;
}

void Wavelet::setTraces(const std::vector<double> traces)
{
    m_traces = traces;
}

unsigned int Wavelet::getFrequency() const
{
    return m_frequency;
}

void Wavelet::setFrequency(const unsigned int frequency)
{
    m_frequency = frequency;
}

} // namespace domain
} // namespace syntheticSeismic
