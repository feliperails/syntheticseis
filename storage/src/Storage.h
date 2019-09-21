#ifndef INC_INVERTSEIS_STORAGE_H
#define INC_INVERTSEIS_STORAGE_H

#include <QObject>

namespace invertseis {
namespace storage {

class Storage : public QObject
{
    Q_OBJECT
public:
    Storage();
    void init();

    Storage& instance();
};


} // namespace storage
} // namespace invertseis
#endif
