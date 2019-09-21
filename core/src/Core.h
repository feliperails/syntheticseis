#ifndef INC_INVERTSEIS_CORE_H
#define INC_INVERTSEIS_CORE_H

#include "CoreDefs.h"

#include <QObject>

namespace invertseis {
namespace core {

class Core : public QObject
{
    Q_OBJECT
public:
    Core();
    void init();

    Core& instance();
};

} // namespace core
} // namespace invertseis
#endif
