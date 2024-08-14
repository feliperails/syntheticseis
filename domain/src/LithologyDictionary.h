#pragma once

#include "Lithology.h"

#include <QVector>

namespace syntheticSeismic {
namespace domain {

class Lithology;
class LithologyDictionaryPrivate;

class LithologyDictionary
{
public:
    const QVector<Lithology> &lithologies() const;

    // Se a litologia não foi encotraa, retorna uma rerência para uma litologia inválida.
    const Lithology& lithology(const int lithologyId) const;

    // Se a litologia não foi encotraa, retorna uma rerência para uma litologia inválida.
    const Lithology& lithology(const QString& lithologyName) const;

    int addLithology(const int id, const QString& name, const double velocity, const double density);

    bool changeData(const int lithologyId, const Lithology& newLithologyData);

    void delLithology(const int id, const QString& name, const double velocity, const double density);

private:
    QVector<Lithology> m_lithologies;
    Lithology m_invalidLithology;
};

} // namespace domain
} // namespace syntheticSeismic
