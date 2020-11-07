#include "StorageTestValues.h"

using namespace syntheticSeismic::domain;

RegularGrid<double> StorageTestValues::regularGridImpedanceFiveFiveFive()
{
    size_t numberOfCellsInX = 5;
    size_t numberOfCellsInY = 5;
    size_t numberOfCellsInZ = 5;
    double cellSizeInX = 1127.7285751357063;
    double cellSizeInY = 2607.6262039715043;
    double cellSizeInZ = 231.61104720000003;
    int noDataValue = 0;
    RegularGrid<double> regularGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
            cellSizeInX, cellSizeInY, cellSizeInZ,
            noDataValue
        );

    regularGrid.setData({
                            {
                                {3, 3, 3.5, 3, 2.8},
                                {3.7, 2.8, 2.8, 2.8, 2.8},
                                {4.5, 3.7, 2.8, 2.8, 2.8},
                                {3, 3.5, 3, 2.8, 3},
                                {3, 2.8, 3.2, 4, 3},
                            },
                            {
                                {3, 2.8, 3.5, 2.8, 2.8},
                                {2.8, 3, 3.5, 3, 2.8},
                                {2.8, 3, 3.2, 2.8, 2.8},
                                {2.8, 3.2, 3.2, 3, 2.8},
                                {3, 3, 3.5, 2.8, 2.8},
                            },
                            {
                                {3, 2.8, 3.2, 2.8, 2.8},
                                {2.8, 3.7, 4, 2.8, 2.8},
                                {3.7, 3.5, 3.5, 2.8, 2.8},
                                {3, 3.5, 3, 3, 3.2},
                                {3.2, 2.8, 2.8, 2.8, 2.8},
                            },
                            {
                                {2.8, 3.7, 2.8, 3, 3},
                                {2.8, 3, 3.7, 2.8, 2.8},
                                {3, 3.5, 3.5, 4, 2.8},
                                {2.8, 2.8, 3.2, 3.5, 3.2},
                                {2.8, 3.5, 3.2, 2.8, 2.8},
                            },
                            {
                                {3, 2.8, 2.8, 2.8, 3},
                                {3, 3.2, 3.2, 2.8, 2.8},
                                {3, 3.2, 4, 3, 2.8},
                                {2.8, 3.2, 2.8, 2.8, 2.8},
                                {3, 2.8, 3.2, 2.8, 2.8},
                            },
                        });

    return regularGrid;
}
