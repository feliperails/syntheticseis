#include <gtest/gtest.h>

#include <data/src/EntityManager.h>
#include <data/mock/DomainObjectMock.h>

TEST(DataTest, EntityTest)
{
    using namespace syntheticSeismic::data;
    Entity e1(QUuid::createUuid(), std::shared_ptr<FirstObjectMock>(), QString("e1"));
    ASSERT_FALSE(e1.domainObject());
    ASSERT_TRUE(!e1.name().isEmpty());
    ASSERT_TRUE(!e1.uuid().isNull());
    ASSERT_EQ(e1.name(), QString("e1"));

    Entity e2(QUuid::createUuid(), std::make_shared<FirstObjectMock>(), QString("e2"));
    ASSERT_TRUE(e2.domainObject());
    ASSERT_TRUE(!e2.name().isEmpty());
    ASSERT_TRUE(!e2.uuid().isNull());
    ASSERT_EQ(e2.name(), QString("e2"));

    Entity e3(QUuid(), std::make_shared<FirstObjectMock>(), QString("e3"));
    ASSERT_TRUE(e3.domainObject());
    ASSERT_TRUE(!e3.name().isEmpty());
    ASSERT_TRUE(e3.uuid().isNull());
    ASSERT_EQ(e3.name(), QString("e3"));
}

TEST(DataTest, EntityManagerTest)
{
    using namespace syntheticSeismic::data;
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

    Entity* firstEntityMock = em.createEntity(QUuid::createUuid(), QString("first_mock"));
    ASSERT_TRUE(firstEntityMock);
    firstEntityMock->setDomainObject(std::make_shared<FirstObjectMock>());

    Entity* secondEntityMock = em.createEntity(QUuid::createUuid(), QString("second_mock"));
    ASSERT_TRUE(secondEntityMock);
    secondEntityMock->setDomainObject(std::make_shared<SecondObjectMock>());

    QLinkedList<Entity*> entities = em.entities<FirstObjectMock>();
    ASSERT_EQ(entities.size(), 1);
    EXPECT_TRUE(std::dynamic_pointer_cast<FirstObjectMock>((*entities.begin())->domainObject()));

    entities = em.entities<SecondObjectMock>();
    ASSERT_EQ(entities.size(), 1);        
    EXPECT_TRUE(std::dynamic_pointer_cast<SecondObjectMock>((*entities.begin())->domainObject()));

    EXPECT_EQ(firstEntityMock, em.entity(QString("first_mock")));
    EXPECT_EQ(firstEntityMock, em.entity(firstEntityMock->uuid()));

    EXPECT_EQ(secondEntityMock, em.entity(QString("second_mock")));
    EXPECT_EQ(secondEntityMock, em.entity(secondEntityMock->uuid()));
}
