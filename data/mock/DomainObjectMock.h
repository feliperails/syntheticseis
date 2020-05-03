#ifndef INC_syntheticSeismic_DATA_DOMAINOBJECTMOCK_H
#define INC_syntheticSeismic_DATA_DOMAINOBJECTMOCK_H

#include <data/src/DomainObject.h>

namespace syntheticSeismic {
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
} // namespace syntheticSeismic
#endif
