#pragma once

#include <QHash>

namespace syntheticSeismic {
namespace domain {

class Lithology;

class SeismicWaveVelocityDictionary
{
public:
    double velocity(const int lithologyID) const;

    bool setVelocity(const int lithologyID, const double velocity);

    QList<int> lithologies() const;

private:
    QHash<int, double> m_data; // QHash<Lithology ID, Seismic velocity>
};

} // namespace domain
} // namespace syntheticSeismic
