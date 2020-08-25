#pragma once

#include <QString>
#include <vector>

namespace syntheticSeismic {
namespace domain {

class Wavelet
{
public:
    Wavelet();
    QString getName() const;
    void setName(const QString &getName);

    double getFirst() const;
    void setFirst(double getFirst);

    double getLast() const;
    void setLast(double getLast);

    unsigned int getStep() const;
    void setStep(unsigned int getStep);

    QString getUnits() const;
    void setUnits(const QString &getUnits);

    unsigned int getFrequency() const;
    void setFrequency(const unsigned int frequency);

    std::vector<double> getTraces() const;
    void setTraces(const std::vector<double> &getTraces);

private:
    QString m_name;
    double m_first;
    double m_last;
    unsigned int m_step;
    unsigned int m_frequency;
    QString m_units;
    std::vector<double> m_traces;
};

} // namespace domain
} // namespace syntheticSeismic
