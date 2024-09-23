#pragma once

#include <QString>

namespace syntheticSeismic {
namespace domain {

enum class FillingType
{
    None,
    Top,
    Bottom
};

class Lithology
{
public:
    // Cria uma litologia inválida. O id será igual a -1.
    Lithology();

    Lithology(const Lithology&);

    Lithology(const int& id, const QString& name, const double& velocity = 0.0, const double& density= 0.0, const FillingType & fillingType = FillingType::None);

    // Se o id for -1, a litologia é inválida.
    const int& getId() const;

    void setId(const int& id);

    const QString& getName() const;

    void setName(const QString& name);

    const double& getVelocity() const;

    void setVelocity(const double& velocity);

    const double& getDensity() const;

    void setDensity(const double& density);

    const FillingType& getFillingType() const;

    void setFillingType(const FillingType& fillingType);

    void coyDataFrom(const Lithology& from);

private:
    int m_id;
    QString m_name;
    double m_velocity;
    double m_density;
    FillingType m_fillingType;
};

} // namespace domain
} // namespace syntheticSeismic
