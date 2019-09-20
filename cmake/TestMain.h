#ifndef DK_TESTMAIN_TESTMAIN_H
#define DK_TESTMAIN_TESTMAIN_H

#include <QObject>
#include <QtTest/QTest>

class TestMain : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void runGTest();
};

#endif
