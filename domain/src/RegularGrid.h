#ifndef INC_INVERTSEIS_DOMAIN_REGULARGRID_H
#define INC_INVERTSEIS_DOMAIN_REGULARGRID_H

#include <data/src/DomainObject.h>

#include <QVector>

namespace invertseis {
namespace domain {

class Cell
{
public:

    Cell();

    Cell(const int id, const int lithologyId);

    int id() const;

    int lithologyId() const;

private:
    int m_id;
    int m_lithologyId;
};

class RegularGrid : public invertseis::data::DomainObject
{
public:
    RegularGrid(const size_t layerCount, const size_t rowCount, const size_t columnCount);

    const Cell& cell(const size_t layer, const size_t row, const size_t column) const;

    void setCell(const size_t layer, const size_t row, const size_t column, const Cell& cell);

//    int index(const int i, const int j) const;

private:
    std::vector<std::vector<std::vector<Cell>>> m_data;
};

} // namespace domain
} // namespace invertseis
#endif
