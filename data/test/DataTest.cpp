#include <gtest/gtest.h>

#include <data/src/EntityManager.h>
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

TEST(DataTest, EntityManagerTest)
{
    using namespace invertseis::data;
    EntityManager em;
    EXPECT_TRUE(em.entities().isEmpty());
    EXPECT_FALSE(em.createEntity(QUuid(), QString()));
    EXPECT_FALSE(em.createEntity(QUuid::createUuid(), QString()));
    EXPECT_FALSE(em.createEntity(QUuid(), QString("test")));
    EXPECT_TRUE(em.entities().isEmpty());

    Entity* e1 = em.createEntity(QUuid::createUuid(), QString("e1"));
    EXPECT_TRUE(e1);

    Entity* e2 = em.createEntity(QUuid::createUuid(), QString("e2"));
    EXPECT_TRUE(e2);

    Entity* e3 = em.createEntity(QUuid::createUuid(), QString("e3"));
    EXPECT_TRUE(e3);

    Entity* e4 = em.createEntity(QUuid::createUuid(), QString("e4"));
    EXPECT_TRUE(e4);

    EXPECT_EQ(em.entities().size(), 4);

    EXPECT_TRUE(em.entities().contains(e1));
    EXPECT_TRUE(em.entities().contains(e2));
    EXPECT_TRUE(em.entities().contains(e3));
    EXPECT_TRUE(em.entities().contains(e4));

    EXPECT_TRUE(em.removeEntity(e1));
    EXPECT_EQ(em.entities().size(), 3);

    EXPECT_FALSE(em.removeEntity(e1));
    EXPECT_EQ(em.entities().size(), 3);
}
