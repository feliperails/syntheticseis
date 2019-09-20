#ifndef INC_INVERTSEIS_DICTIONARY_H
#define INC_INVERTSEIS_DICTIONARY_H

#include <QObject>

#include "DomainObject.h"

namespace invertseis {
namespace data {

class Dictionary : public DomainObject
{
public:
    virtual ~Dictionary();
};

} // namespace data
} // namespace invertseis
#endif
