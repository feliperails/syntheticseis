#include "RegularGrid.h"

namespace invertseis {
namespace domain {

Cell::Cell()
    : m_id(-1)
    , m_lithologyId(-1)
{
}

Cell::Cell(const int id, const int lithologyId)
    : m_id(id)
    , m_lithologyId(lithologyId)
{
}

int Cell::id() const
{
    return m_id;
}

int Cell::lithologyId() const
{
    return m_lithologyId;
}
/*************************************************************/

RegularGrid::RegularGrid(const size_t verticalLayerCount, const size_t rowCount, const size_t columnCount)
    : m_data()
{
    m_data.resize(verticalLayerCount);

    for(size_t layer = 0; layer < verticalLayerCount; ++layer){
        m_data[layer].resize(rowCount);
        for(size_t row = 0; row < rowCount; ++row){
            m_data[layer][row].resize(columnCount);
        }
    }
}

const Cell& RegularGrid::cell(const size_t layer, const size_t row, const size_t column) const
{
    return m_data[layer][row][column];
}

void RegularGrid::setCell(const size_t layer, const size_t row, const size_t column, const Cell& cell)
{
    m_data[layer][row][column] = cell;
}

}
}
