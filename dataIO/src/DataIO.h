#ifndef INC_INVERTSEIS_DATAIO_H
#define INC_INVERTSEIS_DATAIO_H

#include <QObject>

namespace invertseis {
namespace dataIO {

class DataIO : public QObject
{
    Q_OBJECT
public:
    DataIO();

    void init();

    static DataIO& instance();
};

} // namespace dataio
} // namespace invertseis
#endif
