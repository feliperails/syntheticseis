#pragma once

#include <QString>

namespace syntheticSeismic {
namespace domain {

class Lithology
{
public:
    // Cria uma litologia inválida. O id será igual a -1.
    Lithology();

    Lithology(const int id, const QString& name);

    // Se o id for -1, a litologia é inválida.
    int id() const;

    const QString& name() const;

private:
    int m_lithologyId;
    QString m_name;
};

} // namespace domain
} // namespace syntheticSeismic
