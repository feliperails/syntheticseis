#include <gtest/gtest.h>

#include <data/src/Entity.h>
#include <data/mock/DomainObjectMock.h>

TEST(DataTest, EntityDomainObjectRetrieveTest)
{
    using namespace invertseis::data;
    Entity e1(QUuid::createUuid(), std::shared_ptr<DomainObjectMock>(), QString("e1"));
    ASSERT_FALSE(e1.domainObject());
    ASSERT_TRUE(!e1.name().isEmpty());
    ASSERT_TRUE(!e1.uuid().isNull());
    ASSERT_EQ(e1.name(), QString("e1"));

    Entity e2(QUuid::createUuid(), std::make_shared<DomainObjectMock>(), QString("e2"));
    ASSERT_TRUE(e2.domainObject());
    ASSERT_TRUE(!e2.name().isEmpty());
    ASSERT_TRUE(!e2.uuid().isNull());
    ASSERT_EQ(e2.name(), QString("e2"));

    Entity e3(QUuid(), std::make_shared<DomainObjectMock>(), QString("e3"));
    ASSERT_TRUE(e3.domainObject());
    ASSERT_TRUE(!e3.name().isEmpty());
    ASSERT_TRUE(e3.uuid().isNull());
    ASSERT_EQ(e3.name(), QString("e3"));
}
