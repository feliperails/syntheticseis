#ifndef INC_INVERTSEIS_DATA_DOMAINOBJECTMOCK_H
#define INC_INVERTSEIS_DATA_DOMAINOBJECTMOCK_H

#include <data/src/DomainObject.h>

namespace invertseis {
namespace data {

class FirstObjectMock : public data::DomainObject
{
public:
    FirstObjectMock();
};

class SecondObjectMock : public data::DomainObject
{
public:
    SecondObjectMock();
};

} // namespace data
} // namespace invertseis
#endif
