#pragma once

#include <QString>

namespace syntheticSeismic {
namespace domain {

class Lithology
{
public:
    // Cria uma litologia inválida. O id será igual a -1.
    Lithology();

    Lithology(const int id, const QString& name, double velocity = 0.0);

    // Se o id for -1, a litologia é inválida.
    int getId() const;

    const QString& getName() const;

    double getVelocity() const;

    double getDensity() const;

private:
    int m_id;
    QString m_name;
    double m_velocity;
    double m_density;
};

} // namespace domain
} // namespace syntheticSeismic
