#include "TestMain.h"

#include <gtest/gtest.h>
#include <QFile>
#include <QtTest/QtTest>
#include <type_traits>

class GTestExecutionControl : public ::testing::EmptyTestEventListener
{
public:
    ~GTestExecutionControl() override;

    // Fired before the test case starts.
    void OnTestCaseStart(const ::testing::TestCase &test_case) override;

    // Fired before the test starts.
    void OnTestStart(const ::testing::TestInfo &test_info) override;

    // Fired after a failed assertion or a SUCCEED() invocation.
    void OnTestPartResult(const ::testing::TestPartResult &) override;

    // Fired after the test ends.
    void OnTestEnd(const ::testing::TestInfo &test_info) override;

    // Fired after the test case ends.
    void OnTestCaseEnd(const ::testing::TestCase &test_case) override;
};

GTestExecutionControl::~GTestExecutionControl() {}

void GTestExecutionControl::OnTestCaseStart(const testing::TestCase &)
{
}

void GTestExecutionControl::OnTestStart(const testing::TestInfo &)
{
}

void GTestExecutionControl::OnTestPartResult(const ::testing::TestPartResult &r)
{
    if (r.failed()) {
        QFAIL("GTest failed");
    }
}

void GTestExecutionControl::OnTestEnd(const testing::TestInfo &)
{
}

void GTestExecutionControl::OnTestCaseEnd(const testing::TestCase &)
{
}

void TestMain::runGTest()
{
    QCOMPARE(::testing::UnitTest::GetInstance()->Run(), 0);
}

//#if defined(QT_GUI_LIB) || defined(QT_WIDGETS_LIB)

//QT_BEGIN_NAMESPACE
//QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
//QT_END_NAMESPACE
//int main(int argc, char *argv[])
//{
//    ::testing::InitGoogleTest(&argc, argv);
//    ::testing::UnitTest::GetInstance()->listeners().Append(new GTestExecutionControl());

//    QGuiApplication app(argc, argv);
//    app.setAttribute(Qt::AA_Use96Dpi, true);
//    QTEST_ADD_GPU_BLACKLIST_SUPPORT
//    TestMain tc;
//    QTEST_SET_MAIN_SOURCE_PATH
//    return QTest::qExec(&tc, argc, argv);
//}
//#else

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new GTestExecutionControl());

    QCoreApplication app(argc, argv);

//    const QString testFolder = QString("%1/test_folder").arg(app.applicationDirPath());
//    QDir dir;

//    dir.rmpath(testFolder);
//    dir.mkpath(testFolder);
//    dir.cd(testFolder);

    app.setAttribute(Qt::AA_Use96Dpi, true);
    TestMain tc;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&tc, argc, argv);
}

//#endif
