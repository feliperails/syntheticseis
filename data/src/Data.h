#ifndef INC_INVERTSEIS_DATA_DATA_H
#define INC_INVERTSEIS_DATA_DATA_H

#include <QObject>

namespace invertseis {
namespace data {

class Data : public QObject
{
    Q_OBJECT
public:
    Data();
    void init();

    Data& instance();
};

} // namespace data
} // namespace invertseis
#endif
