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
    void setName(const QString &name);

    double getFirst() const;
    void setFirst(double first);

    double getLast() const;
    void setLast(double last);

    unsigned int getStep() const;
    void setStep(unsigned int step);

    QString getUnits() const;
    void setUnits(const QString &units);

    unsigned int getFrequency() const;
    void setFrequency(const unsigned int frequency);

    std::vector<double>& getTraces() const;
    void setTraces(const std::vector<double> traces);

private:
    QString m_name;
    double m_first;
    double m_last;
    unsigned int m_step;
    unsigned int m_frequency;
    QString m_units;
    std::shared_ptr<std::vector<double>> m_traces;
};

} // namespace domain
} // namespace syntheticSeismic
