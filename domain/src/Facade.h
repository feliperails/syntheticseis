#pragma once

#include "LithologyDictionary.h"

#include <QObject>

#include <memory>

namespace syntheticSeismic {
namespace domain {

class Facade: public QObject
{
    Q_OBJECT
public:
    Facade();
    static Facade& instance();

    static LithologyDictionary& lithologyDictionary();

private:
    void init();

private:
    std::unique_ptr<LithologyDictionary> m_lithologyDictionary;
};

} // namespace domain
} // namespace syntheticSeismic
