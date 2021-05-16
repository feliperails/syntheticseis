#include "StorageTestValues.h"

using namespace syntheticSeismic::domain;
using namespace syntheticSeismic::geometry;

RegularGrid<double> StorageTestValues::regularGridImpedanceFiveFiveFive()
{
    size_t numberOfCellsInX = 5;
    size_t numberOfCellsInY = 5;
    size_t numberOfCellsInZ = 5;
    double cellSizeInX = 1127.7285751357063;
    double cellSizeInY = 2607.6262039715043;
    double cellSizeInZ = 231.61104720000003;
    int noDataValue = 0;
    std::array<Point2D, 4> rectanglePoints = {Point2D(0.0, 0.0), Point2D(0.0, 0.0), Point2D(0.0, 0.0), Point2D(0.0, 0.0)};
    RegularGrid<double> regularGrid(
            numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
            cellSizeInX, cellSizeInY, cellSizeInZ,
            rectanglePoints,
            0.0,
            0.0,
            noDataValue,
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

syntheticSeismic::domain::GrdSurface<double> StorageTestValues::simpleGrdSurface()
{
    GrdSurface<double> surface(5, 5);
    surface.setXMin(1000.0);
    surface.setXMax(1400.0);
    surface.setYMin(2000.0);
    surface.setYMax(3200.0);

    const auto ndv = GrdSurface<double>::NoDataValue;
    auto &data = surface.getData();
    data[0] = {1.0, 3.0, 3.0, 5.0, 5.0};
    data[1] = {2.0, 4.0, 3.0, 5.0, 5.0};
    data[2] = {ndv, 4.0, 4.0, 6.0, 6.0};
    data[3] = {ndv, ndv, 4.0, 6.0, 6.0};
    data[4] = {ndv, ndv, ndv, ndv, 6.0};

    return surface;
}
