#ifndef INC_INVERTSEIS_METACLASS_H
#define INC_INVERTSEIS_METACLASS_H

#include <QObject>

namespace invertseis {
namespace core {

class Metaclass
{
public:

    QString toString() const;

private:
    QString m_data;
};

//#define METACLASS(child, parent) private: static Metaclass m_staticMetaclass; public: static MetaClass staticMetaclass{ return m_staticMetaclass; } MetaClass staticMetaclass{ return m_staticMetaclass; }

} // namespace core
} // namespace invertseis
#endif
