#pragma once

#include <QObject>

namespace invertseis {
namespace dataIO {

class Facade : public QObject
{
    Q_OBJECT
public:
    Facade();

    static Facade& instance();
};

} // namespace dataio
} // namespace invertseis

