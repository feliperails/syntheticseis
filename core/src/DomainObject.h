#ifndef INC_INVERTSEIS_DOMAINOBJECT_H
#define INC_INVERTSEIS_DOMAINOBJECT_H

#include <QObject>

#include "Metaclass.h"

namespace invertseis {
namespace core {

class DomainObject
{
public:    
    virtual Metaclass metaclass() const = 0;
};

} // namespace core
} // namespace invertseis
#endif
