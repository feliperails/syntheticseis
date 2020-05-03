#pragma once

#include "data/src/DomainObject.h"

namespace invertseis {
namespace domain {

class Lithology : public data::DomainObject
{
public:

    explicit Lithology(const int id);
    Lithology();

    int id() const;

private:
    int m_lithologyId;
};

} // namespace domain
} // namespace invertseis
