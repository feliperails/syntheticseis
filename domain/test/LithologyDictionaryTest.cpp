#include <gtest/gtest.h>
#include <domain/src/EclipseGridSurface.h>
#include <domain/src/Lithology.h>
#include <domain/src/LithologyDictionary.h>
#include <domain/src/ImpedanceRegularGridCalculator.h>
#include <storage/src/reader/GrdSurfaceReader.h>
#include <QTextStream>

TEST(DomainTest, LithologyDictionaryTest)
{
    using namespace syntheticSeismic::domain;

    LithologyDictionary dictionary;

    dictionary.addLithology(10, QString("e1"), 0.0, 0.0);
    dictionary.addLithology(300, QString("e2"), 0.0, 0.0);
    dictionary.addLithology(8, QString("e3"), 0.0, 0.0);
    dictionary.addLithology(6, QString("e4"), 0.0, 0.0);

    EXPECT_EQ(dictionary.lithology(10).getId(), 10);
    EXPECT_EQ(dictionary.lithology(300).getId(), 300);
    EXPECT_EQ(dictionary.lithology(8).getId(), 8);
    EXPECT_EQ(dictionary.lithology(6).getId(), 6);
    EXPECT_EQ(dictionary.lithology(100).getId(), -1);
    EXPECT_EQ(dictionary.lithology(-1).getId(), -1);

    EXPECT_EQ(dictionary.lithology(QString("e1")).getName(), QString("e1"));
    EXPECT_EQ(dictionary.lithology(QString("e2")).getName(), QString("e2"));
    EXPECT_EQ(dictionary.lithology(QString("e3")).getName(), QString("e3"));
    EXPECT_EQ(dictionary.lithology(QString("e4")).getName(), QString("e4"));
    EXPECT_EQ(dictionary.lithology(QString("e5")).getName(), QString());
    EXPECT_EQ(dictionary.lithology(QString()).getName(), QString());

    EXPECT_EQ(dictionary.lithologies().size(), 4);
}