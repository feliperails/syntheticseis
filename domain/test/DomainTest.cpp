#include <gtest/gtest.h>

#include <domain/src/SeismicWaveVelocityDictionary.h>
#include <domain/src/Lithology.h>

#include <data/src/Entity.h>
#include <data/mock/DomainObjectMock.h>

TEST(DomainTest, SeismicWaveVelocityDictionaryTest)
{
    using namespace invertseis::domain;
    using namespace invertseis::data;

    Entity e1(QUuid::createUuid(), std::make_shared<Lithology>(), QString("e1"));
    Entity e2(QUuid::createUuid(), std::make_shared<Lithology>(), QString("e2"));
    Entity e3(QUuid::createUuid(), std::make_shared<Lithology>(), QString("e3"));
    Entity e4(QUuid::createUuid(), std::make_shared<invertseis::data::DomainObjectMock>(), QString("e4"));

    SeismicWaveVelocityDictionary dictionary;
    ASSERT_EQ(dictionary.lithologyEntities().size(), 0);
    ASSERT_EQ(dictionary.lithologyEntities(), QList<const Entity*>());

    ASSERT_TRUE(dictionary.setVelocity(&e1, 100.5));
    ASSERT_TRUE(dictionary.setVelocity(&e2, 200.0));
    ASSERT_TRUE(dictionary.setVelocity(&e3, 300.0));
    ASSERT_FALSE(dictionary.setVelocity(&e4, 400.0));

    ASSERT_TRUE(qFuzzyCompare(dictionary.velocity(&e1), 100.5));
    ASSERT_TRUE(qFuzzyCompare(dictionary.velocity(&e2), 200.0));
    ASSERT_TRUE(qFuzzyCompare(dictionary.velocity(&e3), 300.0));
    ASSERT_FALSE(qFuzzyCompare(dictionary.velocity(&e4), 400.0));

    ASSERT_FALSE(qFuzzyCompare(dictionary.velocity(&e1), 100.0));

    ASSERT_TRUE(qFuzzyIsNull(dictionary.velocity(&e4)));
    ASSERT_TRUE(qFuzzyIsNull(dictionary.velocity(nullptr)));
    ASSERT_FALSE(qFuzzyCompare(dictionary.velocity(nullptr), 100.0));
}
