#include <gmock/gmock.h>

#include <QString>

#include <iostream>
#include <vector>

namespace syntheticSeismic {
namespace domain {
namespace mock {

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Invoke;
using ::testing::_;
using ::testing::NiceMock;

class IExample
{
public:
    virtual ~IExample() {}

    virtual bool actionA() const = 0;
    virtual bool actionB(const double value) const = 0;

    virtual void updateA();
    virtual void updateB(const double value) = 0;

    virtual std::vector<int> make() = 0;

    virtual void create(const int a, const int b, const int c) = 0;
};

class Dog
{
public:
    void setName(){
        std::cout << "Dog::setName"  << std::endl;
    }

    void setAge(const double age){
        std::cout << "Age: " << age << std::endl;
    }

    bool bark(){
        std::cout << "Au au au" << std::endl;
        return true;
    }

    QString callHuman() {
        return QLatin1String("Human");
    }

    bool sleep(const double hours){
        std::cout << "Sleeping for " << hours << std::endl;
        return true;
    }

    std::vector<int> children() {
        return {1, 2, 3, 4, 5};
    }
};

class Example : IExample
{
public:
    Example()
        : m_dog()
    {
        setDefaultExpectations();
    }

public:
    MOCK_CONST_METHOD0(actionA, bool());
    MOCK_CONST_METHOD1(actionB, bool(const double));
    MOCK_METHOD0(updateA, void());
    MOCK_METHOD1(updateB, void(const double));
    MOCK_METHOD0(make, std::vector<int>());
    MOCK_METHOD3(create, void(int, int, int));

    void doCreate(const int a, const int b, const int c)
    {
        std::cout << a << b << c << std::endl;
    }

    void doCreateFailed(const int, const int, const int)
    {
        std::cout << "doCreateFailed" << std::endl;
    }

    void updateAFailed()
    {
        std::cout << "Update A failed" <<std::endl;
    }

    void updateBFailed(const double)
    {
        std::cout << "Update B failed" <<std::endl;
    }

    void setDefaultExpectations()
    {
        EXPECT_CALL(*this, actionA()).WillRepeatedly(Invoke(&m_dog, &Dog::bark));
        EXPECT_CALL(*this, actionB(_)).WillRepeatedly(Invoke(&m_dog, &Dog::sleep));
        EXPECT_CALL(*this, updateA()).WillRepeatedly(Invoke(&m_dog, &Dog::setName));
        EXPECT_CALL(*this, updateB(_)).WillRepeatedly(Invoke(&m_dog, &Dog::setAge));
        EXPECT_CALL(*this, make()).WillRepeatedly(Invoke(&m_dog, &Dog::children));
        EXPECT_CALL(*this, create(_, _, _)).WillRepeatedly(Invoke(this, &Example::doCreate));
    }

    void setFailureExpectations()
    {
        EXPECT_CALL(*this, actionA()).WillRepeatedly(Return(false));
        EXPECT_CALL(*this, actionB(_)).WillRepeatedly(Return(false));
        EXPECT_CALL(*this, updateA()).WillRepeatedly(Invoke(this, &Example::updateAFailed));
        EXPECT_CALL(*this, updateB(_)).WillRepeatedly(Invoke(this, &Example::updateBFailed));
        EXPECT_CALL(*this, make()).WillRepeatedly(Return(std::vector<int>()));
        EXPECT_CALL(*this, create(_, _, _)).WillRepeatedly(Invoke(this, &Example::doCreateFailed));
    }
private:
    Dog m_dog;
};

} // namespace mock
} // namespace domain
} // namespace syntheticSeismic
