#pragma once

#include <QObject>

namespace invertseis {
namespace  storage {

class Facade : public QObject
{
    Q_OBJECT
public:
    Facade();

    static Facade& instance();
};

} // namespace storage
} // namespace invertseis

