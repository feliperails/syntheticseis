#include "DomainTestValues.h"
#include <geometry/src/Coordinate.h>
#include <domain/src/VolumeToRegularGrid.h>

using namespace syntheticSeismic::geometry;
using namespace syntheticSeismic::domain;

EclipseGrid DomainTestValues::eclipseGridFromSimpleGrid()
{
    const size_t numberOfCellsInX = 2;
    const size_t numberOfCellsInY = 3;
    const size_t numberOfCellsInZ = 4;

    const std::vector<Coordinate> coordinates = {
        Coordinate(1000, 2000, 1000),
        Coordinate(1100, 2000, 1100),
        Coordinate(1050, 2000, 1000),
        Coordinate(1150, 2000, 1100),
        Coordinate(1100, 2000, 1000),
        Coordinate(1200, 2000, 1100),
        Coordinate(1000, 2200, 1000),
        Coordinate(1100, 2200, 1100),
        Coordinate(1100, 2200, 1000),
        Coordinate(1200, 2200, 1100),
        Coordinate(1200, 2200, 1000),
        Coordinate(1300, 2200, 1100),
        Coordinate(1000, 2600, 1000),
        Coordinate(1100, 2600, 1100),
        Coordinate(1150, 2600, 1000),
        Coordinate(1250, 2600, 1100),
        Coordinate(1300, 2600, 1000),
        Coordinate(1400, 2600, 1100),
        Coordinate(1000, 3200, 1000),
        Coordinate(1100, 3200, 1100),
        Coordinate(1200, 3200, 1000),
        Coordinate(1300, 3200, 1100),
        Coordinate(1400, 3200, 1000),
        Coordinate(1500, 3200, 1100)
    };

    const std::vector<double> zCoordinates = {
        1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
        1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
        1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
        1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
        1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
        1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300,
        1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300,
        1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400
    };

    const std::vector<int> lithologyIds = {
         1,  2,  3,  4,  5,  6,
         7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24
    };

    return EclipseGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ, coordinates, zCoordinates, lithologyIds);
}

std::vector<std::shared_ptr<Volume>> DomainTestValues::volumesOfFirstLayerFromSimpleGrid()
{
    std::vector<std::shared_ptr<Volume>> volumes(6);
    for (size_t i = 0; i < volumes.size(); ++i)
    {
        volumes[i] = std::make_shared<Volume>(i);
    }
    volumes[0]->points = {
        Point3D(1000, 2000, 1000),
        Point3D(1050, 2000, 1000),
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1100, 2000, 1100),
        Point3D(1150, 2000, 1100),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
    };
    volumes[1]->points = {
        Point3D(1050, 2000, 1000),
        Point3D(1100, 2000, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2000, 1100),
        Point3D(1200, 2000, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
    };
    volumes[2]->points = {
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
    };
    volumes[3]->points = {
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
    };
    volumes[4]->points = {
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1000, 3200, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1100, 3200, 1100),
        Point3D(1300, 3200, 1100),
    };
    volumes[5]->points = {
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1400, 3200, 1000),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
        Point3D(1300, 3200, 1100),
        Point3D(1500, 3200, 1100),
    };

    return volumes;
}

std::vector<std::shared_ptr<Volume>> DomainTestValues::volumesFromSimpleGrid()
{
    std::vector<std::shared_ptr<Volume>> volumes(24);
    for (size_t i = 0; i < volumes.size(); ++i)
    {
        volumes[i] = std::make_shared<Volume>(i);
    }
    volumes[0]->points = {
        Point3D(1000, 2000, 1000),
        Point3D(1050, 2000, 1000),
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1100, 2000, 1100),
        Point3D(1150, 2000, 1100),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
    };
    volumes[1]->points = {
        Point3D(1050, 2000, 1000),
        Point3D(1100, 2000, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2000, 1100),
        Point3D(1200, 2000, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
    };
    volumes[2]->points = {
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
    };
    volumes[3]->points = {
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
    };
    volumes[4]->points = {
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1000, 3200, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1100, 3200, 1100),
        Point3D(1300, 3200, 1100),
    };
    volumes[5]->points = {
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1400, 3200, 1000),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
        Point3D(1300, 3200, 1100),
        Point3D(1500, 3200, 1100),
    };
    volumes[6]->points = {
        Point3D(1100, 2000, 1100),
        Point3D(1150, 2000, 1100),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1200, 2000, 1200),
        Point3D(1250, 2000, 1200),
        Point3D(1200, 2200, 1200),
        Point3D(1300, 2200, 1200),
    };
    volumes[7]->points = {
        Point3D(1150, 2000, 1100),
        Point3D(1200, 2000, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1250, 2000, 1200),
        Point3D(1300, 2000, 1200),
        Point3D(1300, 2200, 1200),
        Point3D(1400, 2200, 1200),
    };
    volumes[8]->points = {
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1200, 2200, 1200),
        Point3D(1300, 2200, 1200),
        Point3D(1200, 2600, 1200),
        Point3D(1350, 2600, 1200),
    };
    volumes[9]->points = {
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
        Point3D(1300, 2200, 1200),
        Point3D(1400, 2200, 1200),
        Point3D(1350, 2600, 1200),
        Point3D(1500, 2600, 1200),
    };
    volumes[10]->points = {
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1100, 3200, 1100),
        Point3D(1300, 3200, 1100),
        Point3D(1200, 2600, 1200),
        Point3D(1350, 2600, 1200),
        Point3D(1200, 3200, 1200),
        Point3D(1400, 3200, 1200),
    };
    volumes[11]->points = {
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
        Point3D(1300, 3200, 1100),
        Point3D(1500, 3200, 1100),
        Point3D(1350, 2600, 1200),
        Point3D(1500, 2600, 1200),
        Point3D(1400, 3200, 1200),
        Point3D(1600, 3200, 1200),
    };
    volumes[12]->points = {
        Point3D(1200, 2000, 1200),
        Point3D(1250, 2000, 1200),
        Point3D(1200, 2200, 1200),
        Point3D(1300, 2200, 1200),
        Point3D(1300, 2000, 1300),
        Point3D(1350, 2000, 1300),
        Point3D(1300, 2200, 1300),
        Point3D(1400, 2200, 1300),
    };
    volumes[13]->points = {
        Point3D(1250, 2000, 1200),
        Point3D(1300, 2000, 1200),
        Point3D(1300, 2200, 1200),
        Point3D(1400, 2200, 1200),
        Point3D(1350, 2000, 1300),
        Point3D(1400, 2000, 1300),
        Point3D(1400, 2200, 1300),
        Point3D(1500, 2200, 1300),
    };
    volumes[14]->points = {
        Point3D(1200, 2200, 1200),
        Point3D(1300, 2200, 1200),
        Point3D(1200, 2600, 1200),
        Point3D(1350, 2600, 1200),
        Point3D(1300, 2200, 1300),
        Point3D(1400, 2200, 1300),
        Point3D(1300, 2600, 1300),
        Point3D(1450, 2600, 1300),
    };
    volumes[15]->points = {
        Point3D(1300, 2200, 1200),
        Point3D(1400, 2200, 1200),
        Point3D(1350, 2600, 1200),
        Point3D(1500, 2600, 1200),
        Point3D(1400, 2200, 1300),
        Point3D(1500, 2200, 1300),
        Point3D(1450, 2600, 1300),
        Point3D(1600, 2600, 1300),
    };
    volumes[16]->points = {
        Point3D(1200, 2600, 1200),
        Point3D(1350, 2600, 1200),
        Point3D(1200, 3200, 1200),
        Point3D(1400, 3200, 1200),
        Point3D(1300, 2600, 1300),
        Point3D(1450, 2600, 1300),
        Point3D(1300, 3200, 1300),
        Point3D(1500, 3200, 1300),
    };
    volumes[17]->points = {
        Point3D(1350, 2600, 1200),
        Point3D(1500, 2600, 1200),
        Point3D(1400, 3200, 1200),
        Point3D(1600, 3200, 1200),
        Point3D(1450, 2600, 1300),
        Point3D(1600, 2600, 1300),
        Point3D(1500, 3200, 1300),
        Point3D(1700, 3200, 1300),
    };
    volumes[18]->points = {
        Point3D(1300, 2000, 1300),
        Point3D(1350, 2000, 1300),
        Point3D(1300, 2200, 1300),
        Point3D(1400, 2200, 1300),
        Point3D(1400, 2000, 1400),
        Point3D(1450, 2000, 1400),
        Point3D(1400, 2200, 1400),
        Point3D(1500, 2200, 1400),
    };
    volumes[19]->points = {
        Point3D(1350, 2000, 1300),
        Point3D(1400, 2000, 1300),
        Point3D(1400, 2200, 1300),
        Point3D(1500, 2200, 1300),
        Point3D(1450, 2000, 1400),
        Point3D(1500, 2000, 1400),
        Point3D(1500, 2200, 1400),
        Point3D(1600, 2200, 1400),
    };
    volumes[20]->points = {
        Point3D(1300, 2200, 1300),
        Point3D(1400, 2200, 1300),
        Point3D(1300, 2600, 1300),
        Point3D(1450, 2600, 1300),
        Point3D(1400, 2200, 1400),
        Point3D(1500, 2200, 1400),
        Point3D(1400, 2600, 1400),
        Point3D(1550, 2600, 1400),
    };
    volumes[21]->points = {
        Point3D(1400, 2200, 1300),
        Point3D(1500, 2200, 1300),
        Point3D(1450, 2600, 1300),
        Point3D(1600, 2600, 1300),
        Point3D(1500, 2200, 1400),
        Point3D(1600, 2200, 1400),
        Point3D(1550, 2600, 1400),
        Point3D(1700, 2600, 1400),
    };
    volumes[22]->points = {
        Point3D(1300, 2600, 1300),
        Point3D(1450, 2600, 1300),
        Point3D(1300, 3200, 1300),
        Point3D(1500, 3200, 1300),
        Point3D(1400, 2600, 1400),
        Point3D(1550, 2600, 1400),
        Point3D(1400, 3200, 1400),
        Point3D(1600, 3200, 1400),
    };
    volumes[23]->points = {
        Point3D(1450, 2600, 1300),
        Point3D(1600, 2600, 1300),
        Point3D(1500, 3200, 1300),
        Point3D(1700, 3200, 1300),
        Point3D(1550, 2600, 1400),
        Point3D(1700, 2600, 1400),
        Point3D(1600, 3200, 1400),
        Point3D(1800, 3200, 1400),
    };

    return volumes;
}



std::vector<Point2D> DomainTestValues::minimumRectangleFromSimpleGrid()
{
    return {
        {1800, 2000},
        {1800, 3200},
        {1000, 3200},
        {1000, 2000}
    };
}

std::vector<std::shared_ptr<Volume>> DomainTestValues::volumesFromSimpleGridRotated30Degrees()
{
    std::vector<std::shared_ptr<Volume>> volumes(24);
    for (size_t i = 0; i < volumes.size(); ++i)
    {
        volumes[i] = std::make_shared<Volume>(i);
    }
    volumes[0]->points = {
        Point3D(-133.975, 2232.05, 1000),
        Point3D(-90.6738, 2257.05, 1000),
        Point3D(-233.975, 2405.26, 1000),
        Point3D(-147.373, 2455.26, 1000),
        Point3D(-47.3726, 2282.05, 1100),
        Point3D(-4.0713, 2307.05, 1100),
        Point3D(-147.373, 2455.26, 1100),
        Point3D(-60.7701, 2505.26, 1100),
    };
    volumes[1]->points = {
        Point3D(-90.6738, 2257.05, 1000),
        Point3D(-47.3726, 2282.05, 1000),
        Point3D(-147.373, 2455.26, 1000),
        Point3D(-60.7701, 2505.26, 1000),
        Point3D(-4.0713, 2307.05, 1100),
        Point3D(39.23, 2332.05, 1100),
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(25.8325, 2555.26, 1100),
    };
    volumes[2]->points = {
        Point3D(-233.975, 2405.26, 1000),
        Point3D(-147.373, 2455.26, 1000),
        Point3D(-433.975, 2751.67, 1000),
        Point3D(-304.071, 2826.67, 1000),
        Point3D(-147.373, 2455.26, 1100),
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(-347.373, 2801.67, 1100),
        Point3D(-217.469, 2876.67, 1100),
    };
    volumes[3]->points = {
        Point3D(-147.373, 2455.26, 1000),
        Point3D(-60.7701, 2505.26, 1000),
        Point3D(-304.071, 2826.67, 1000),
        Point3D(-174.168, 2901.67, 1000),
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(25.8325, 2555.26, 1100),
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-87.5651, 2951.67, 1100),
    };
    volumes[4]->points = {
        Point3D(-433.975, 2751.67, 1000),
        Point3D(-304.071, 2826.67, 1000),
        Point3D(-733.975, 3271.28, 1000),
        Point3D(-560.77, 3371.28, 1000),
        Point3D(-347.373, 2801.67, 1100),
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-647.373, 3321.28, 1100),
        Point3D(-474.168, 3421.28, 1100),
    };
    volumes[5]->points = {
        Point3D(-304.071, 2826.67, 1000),
        Point3D(-174.168, 2901.67, 1000),
        Point3D(-560.77, 3371.28, 1000),
        Point3D(-387.565, 3471.28, 1000),
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-87.5651, 2951.67, 1100),
        Point3D(-474.168, 3421.28, 1100),
        Point3D(-300.963, 3521.28, 1100),
    };
    volumes[6]->points = {
        Point3D(-47.3726, 2282.05, 1100),
        Point3D(-4.0713, 2307.05, 1100),
        Point3D(-147.373, 2455.26, 1100),
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(39.23, 2332.05, 1200),
        Point3D(82.5312, 2357.05, 1200),
        Point3D(-60.7701, 2505.26, 1200),
        Point3D(25.8325, 2555.26, 1200),
    };
    volumes[7]->points = {
        Point3D(-4.0713, 2307.05, 1100),
        Point3D(39.23, 2332.05, 1100),
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(25.8325, 2555.26, 1100),
        Point3D(82.5312, 2357.05, 1200),
        Point3D(125.832, 2382.05, 1200),
        Point3D(25.8325, 2555.26, 1200),
        Point3D(112.435, 2605.26, 1200),
    };
    volumes[8]->points = {
        Point3D(-147.373, 2455.26, 1100),
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(-347.373, 2801.67, 1100),
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-60.7701, 2505.26, 1200),
        Point3D(25.8325, 2555.26, 1200),
        Point3D(-260.77, 2851.67, 1200),
        Point3D(-130.866, 2926.67, 1200),
    };
    volumes[9]->points = {
        Point3D(-60.7701, 2505.26, 1100),
        Point3D(25.8325, 2555.26, 1100),
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-87.5651, 2951.67, 1100),
        Point3D(25.8325, 2555.26, 1200),
        Point3D(112.435, 2605.26, 1200),
        Point3D(-130.866, 2926.67, 1200),
        Point3D(-0.96257, 3001.67, 1200),
    };
    volumes[10]->points = {
        Point3D(-347.373, 2801.67, 1100),
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-647.373, 3321.28, 1100),
        Point3D(-474.168, 3421.28, 1100),
        Point3D(-260.77, 2851.67, 1200),
        Point3D(-130.866, 2926.67, 1200),
        Point3D(-560.77, 3371.28, 1200),
        Point3D(-387.565, 3471.28, 1200),
    };
    volumes[11]->points = {
        Point3D(-217.469, 2876.67, 1100),
        Point3D(-87.5651, 2951.67, 1100),
        Point3D(-474.168, 3421.28, 1100),
        Point3D(-300.963, 3521.28, 1100),
        Point3D(-130.866, 2926.67, 1200),
        Point3D(-0.96257, 3001.67, 1200),
        Point3D(-387.565, 3471.28, 1200),
        Point3D(-214.36, 3571.28, 1200),
    };
    volumes[12]->points = {
        Point3D(39.23, 2332.05, 1200),
        Point3D(82.5312, 2357.05, 1200),
        Point3D(-60.7701, 2505.26, 1200),
        Point3D(25.8325, 2555.26, 1200),
        Point3D(125.832, 2382.05, 1300),
        Point3D(169.134, 2407.05, 1300),
        Point3D(25.8325, 2555.26, 1300),
        Point3D(112.435, 2605.26, 1300),
    };
    volumes[13]->points = {
        Point3D(82.5312, 2357.05, 1200),
        Point3D(125.832, 2382.05, 1200),
        Point3D(25.8325, 2555.26, 1200),
        Point3D(112.435, 2605.26, 1200),
        Point3D(169.134, 2407.05, 1300),
        Point3D(212.435, 2432.05, 1300),
        Point3D(112.435, 2605.26, 1300),
        Point3D(199.038, 2655.26, 1300),
    };
    volumes[14]->points = {
        Point3D(-60.7701, 2505.26, 1200),
        Point3D(25.8325, 2555.26, 1200),
        Point3D(-260.77, 2851.67, 1200),
        Point3D(-130.866, 2926.67, 1200),
        Point3D(25.8325, 2555.26, 1300),
        Point3D(112.435, 2605.26, 1300),
        Point3D(-174.168, 2901.67, 1300),
        Point3D(-44.2638, 2976.67, 1300),
    };
    volumes[15]->points = {
        Point3D(25.8325, 2555.26, 1200),
        Point3D(112.435, 2605.26, 1200),
        Point3D(-130.866, 2926.67, 1200),
        Point3D(-0.96257, 3001.67, 1200),
        Point3D(112.435, 2605.26, 1300),
        Point3D(199.038, 2655.26, 1300),
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(85.64, 3051.67, 1300),
    };
    volumes[16]->points = {
        Point3D(-260.77, 2851.67, 1200),
        Point3D(-130.866, 2926.67, 1200),
        Point3D(-560.77, 3371.28, 1200),
        Point3D(-387.565, 3471.28, 1200),
        Point3D(-174.168, 2901.67, 1300),
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(-474.168, 3421.28, 1300),
        Point3D(-300.963, 3521.28, 1300),
    };
    volumes[17]->points = {
        Point3D(-130.866, 2926.67, 1200),
        Point3D(-0.96257, 3001.67, 1200),
        Point3D(-387.565, 3471.28, 1200),
        Point3D(-214.36, 3571.28, 1200),
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(85.64, 3051.67, 1300),
        Point3D(-300.963, 3521.28, 1300),
        Point3D(-127.758, 3621.28, 1300),
    };
    volumes[18]->points = {
        Point3D(125.832, 2382.05, 1300),
        Point3D(169.134, 2407.05, 1300),
        Point3D(25.8325, 2555.26, 1300),
        Point3D(112.435, 2605.26, 1300),
        Point3D(212.435, 2432.05, 1400),
        Point3D(255.736, 2457.05, 1400),
        Point3D(112.435, 2605.26, 1400),
        Point3D(199.038, 2655.26, 1400),
    };
    volumes[19]->points = {
        Point3D(169.134, 2407.05, 1300),
        Point3D(212.435, 2432.05, 1300),
        Point3D(112.435, 2605.26, 1300),
        Point3D(199.038, 2655.26, 1300),
        Point3D(255.736, 2457.05, 1400),
        Point3D(299.038, 2482.05, 1400),
        Point3D(199.038, 2655.26, 1400),
        Point3D(285.64, 2705.26, 1400),
    };
    volumes[20]->points = {
        Point3D(25.8325, 2555.26, 1300),
        Point3D(112.435, 2605.26, 1300),
        Point3D(-174.168, 2901.67, 1300),
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(112.435, 2605.26, 1400),
        Point3D(199.038, 2655.26, 1400),
        Point3D(-87.5651, 2951.67, 1400),
        Point3D(42.3387, 3026.67, 1400),
    };
    volumes[21]->points = {
        Point3D(112.435, 2605.26, 1300),
        Point3D(199.038, 2655.26, 1300),
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(85.64, 3051.67, 1300),
        Point3D(199.038, 2655.26, 1400),
        Point3D(285.64, 2705.26, 1400),
        Point3D(42.3387, 3026.67, 1400),
        Point3D(172.242, 3101.67, 1400),
    };
    volumes[22]->points = {
        Point3D(-174.168, 2901.67, 1300),
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(-474.168, 3421.28, 1300),
        Point3D(-300.963, 3521.28, 1300),
        Point3D(-87.5651, 2951.67, 1400),
        Point3D(42.3387, 3026.67, 1400),
        Point3D(-387.565, 3471.28, 1400),
        Point3D(-214.36, 3571.28, 1400),
    };
    volumes[23]->points = {
        Point3D(-44.2638, 2976.67, 1300),
        Point3D(85.64, 3051.67, 1300),
        Point3D(-300.963, 3521.28, 1300),
        Point3D(-127.758, 3621.28, 1300),
        Point3D(42.3387, 3026.67, 1400),
        Point3D(172.242, 3101.67, 1400),
        Point3D(-214.36, 3571.28, 1400),
        Point3D(-41.1551, 3671.28, 1400),
    };

    return volumes;
}

std::pair<Point2D, double> DomainTestValues::referencePointAndAngleInRadiansFromSimpleGridRotated30Degrees()
{
    return {
        {-733.9751250000876, 3271.2802165064018},
        1.0471973493043079
    };
}

std::vector<std::shared_ptr<Volume>> DomainTestValues::unrotatedVolumesFromSimpleGridRotated30Degrees()
{
    std::vector<std::shared_ptr<Volume>> volumes(24);
    for (size_t i = 0; i < volumes.size(); ++i)
    {
        volumes[i] = std::make_shared<Volume>(i);
    }
    volumes[0]->points = {
        Point3D(1199.99984, 0.00001, 0.00000),
        Point3D(1199.99981, 49.99994, 0.00000),
        Point3D(999.99558, 0.00251, 0.00000),
        Point3D(999.99533, 100.00204, 0.00000),
        Point3D(1199.99979, 99.99988, 100.00000),
        Point3D(1199.99981, 149.99991, 100.00000),
        Point3D(999.99533, 100.00204, 100.00000),
        Point3D(999.99553, 200.00235, 100.00000),
    };
    volumes[0]->idLithology = 1;

    volumes[1]->points = {
        Point3D(1199.99981, 49.99994, 0.00000),
        Point3D(1199.99979, 99.99988, 0.00000),
        Point3D(999.99533, 100.00204, 0.00000),
        Point3D(999.99553, 200.00235, 0.00000),
        Point3D(1199.99981, 149.99991, 100.00000),
        Point3D(1199.99984, 199.99994, 100.00000),
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(999.99558, 300.00240, 100.00000),
    };
    volumes[1]->idLithology = 3;

    volumes[2]->points = {
        Point3D(999.99558, 0.00251, 0.00000),
        Point3D(999.99533, 100.00204, 0.00000),
        Point3D(599.99572, 0.00251, 0.00000),
        Point3D(599.99584, 150.00267, 0.00000),
        Point3D(999.99533, 100.00204, 100.00000),
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(599.99547, 100.00204, 100.00000),
        Point3D(599.99559, 250.00220, 100.00000),
    };
    volumes[2]->idLithology = 5;

    volumes[3]->points = {
        Point3D(999.99533, 100.00204, 0.00000),
        Point3D(999.99553, 200.00235, 0.00000),
        Point3D(599.99584, 150.00267, 0.00000),
        Point3D(599.99547, 300.00197, 0.00000),
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(999.99558, 300.00240, 100.00000),
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(599.99567, 400.00228, 100.00000),
    };
    volumes[3]->idLithology = 7;

    volumes[4]->points = {
        Point3D(599.99572, 0.00251, 0.00000),
        Point3D(599.99584, 150.00267, 0.00000),
        Point3D(0.00026, 0.00001, 0.00000),
        Point3D(0.00026, 199.99994, 0.00000),
        Point3D(599.99547, 100.00204, 100.00000),
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(0.00001, 99.99954, 100.00000),
        Point3D(0.00001, 299.99947, 100.00000),
    };
    volumes[4]->idLithology = 9;

    volumes[5]->points = {
        Point3D(599.99584, 150.00267, 0.00000),
        Point3D(599.99547, 300.00197, 0.00000),
        Point3D(0.00026, 199.99994, 0.00000),
        Point3D(0.00026, 399.99987, 0.00000),
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(599.99567, 400.00228, 100.00000),
        Point3D(0.00001, 299.99947, 100.00000),
        Point3D(0.00001, 499.99940, 100.00000),
    };
    volumes[5]->idLithology = 11;

    volumes[6]->points = {
        Point3D(1199.99979, 99.99988, 100.00000),
        Point3D(1199.99981, 149.99991, 100.00000),
        Point3D(999.99533, 100.00204, 100.00000),
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(1199.99984, 199.99994, 200.00000),
        Point3D(1199.99981, 249.99987, 200.00000),
        Point3D(999.99553, 200.00235, 200.00000),
        Point3D(999.99558, 300.00240, 200.00000),
    };
    volumes[6]->idLithology = 13;

    volumes[7]->points = {
        Point3D(1199.99981, 149.99991, 100.00000),
        Point3D(1199.99984, 199.99994, 100.00000),
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(999.99558, 300.00240, 100.00000),
        Point3D(1199.99981, 249.99987, 200.00000),
        Point3D(1199.99959, 299.99947, 200.00000),
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(999.99558, 400.00237, 200.00000),
    };
    volumes[7]->idLithology = 15;

    volumes[8]->points = {
        Point3D(999.99533, 100.00204, 100.00000),
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(599.99547, 100.00204, 100.00000),
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(999.99553, 200.00235, 200.00000),
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(599.99572, 200.00244, 200.00000),
        Point3D(599.99584, 350.00260, 200.00000),
    };
    volumes[8]->idLithology = 17;

    volumes[9]->points = {
        Point3D(999.99553, 200.00235, 100.00000),
        Point3D(999.99558, 300.00240, 100.00000),
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(599.99567, 400.00228, 100.00000),
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(999.99558, 400.00237, 200.00000),
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(599.99568, 500.00227, 200.00000),
    };
    volumes[9]->idLithology = 24;

    volumes[10]->points = {
        Point3D(599.99547, 100.00204, 100.00000),
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(0.00001, 99.99954, 100.00000),
        Point3D(0.00001, 299.99947, 100.00000),
        Point3D(599.99572, 200.00244, 200.00000),
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(0.00026, 199.99994, 200.00000),
        Point3D(0.00026, 399.99987, 200.00000),
    };
    volumes[10]->idLithology = 1;

    volumes[11]->points = {
        Point3D(599.99559, 250.00220, 100.00000),
        Point3D(599.99567, 400.00228, 100.00000),
        Point3D(0.00001, 299.99947, 100.00000),
        Point3D(0.00001, 499.99940, 100.00000),
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(599.99568, 500.00227, 200.00000),
        Point3D(0.00026, 399.99987, 200.00000),
        Point3D(0.00026, 599.99980, 200.00000),
    };
    volumes[11]->idLithology = 3;

    volumes[12]->points = {
        Point3D(1199.99984, 199.99994, 200.00000),
        Point3D(1199.99981, 249.99987, 200.00000),
        Point3D(999.99553, 200.00235, 200.00000),
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(1199.99959, 299.99947, 300.00000),
        Point3D(1199.99996, 350.00010, 300.00000),
        Point3D(999.99558, 300.00240, 300.00000),
        Point3D(999.99558, 400.00237, 300.00000),
    };
    volumes[12]->idLithology = 5;

    volumes[13]->points = {
        Point3D(1199.99981, 249.99987, 200.00000),
        Point3D(1199.99959, 299.99947, 200.00000),
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(999.99558, 400.00237, 200.00000),
        Point3D(1199.99996, 350.00010, 300.00000),
        Point3D(1199.99984, 399.99987, 300.00000),
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(999.99583, 500.00276, 300.00000),
    };
    volumes[13]->idLithology = 7;

    volumes[14]->points = {
        Point3D(999.99553, 200.00235, 200.00000),
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(599.99572, 200.00244, 200.00000),
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(999.99558, 300.00240, 300.00000),
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(599.99547, 300.00197, 300.00000),
        Point3D(599.99569, 450.00230, 300.00000),
    };
    volumes[14]->idLithology = 9;

    volumes[15]->points = {
        Point3D(999.99558, 300.00240, 200.00000),
        Point3D(999.99558, 400.00237, 200.00000),
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(599.99568, 500.00227, 200.00000),
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(999.99583, 500.00276, 300.00000),
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(599.99572, 600.00230, 300.00000),
    };
    volumes[15]->idLithology = 11;

    volumes[16]->points = {
        Point3D(599.99572, 200.00244, 200.00000),
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(0.00026, 199.99994, 200.00000),
        Point3D(0.00026, 399.99987, 200.00000),
        Point3D(599.99547, 300.00197, 300.00000),
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(0.00001, 299.99947, 300.00000),
        Point3D(0.00001, 499.99940, 300.00000),
    };
    volumes[16]->idLithology = 13;

    volumes[17]->points = {
        Point3D(599.99584, 350.00260, 200.00000),
        Point3D(599.99568, 500.00227, 200.00000),
        Point3D(0.00026, 399.99987, 200.00000),
        Point3D(0.00026, 599.99980, 200.00000),
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(599.99572, 600.00230, 300.00000),
        Point3D(0.00001, 499.99940, 300.00000),
        Point3D(0.00001, 699.99933, 300.00000),
    };
    volumes[17]->idLithology = 15;

    volumes[18]->points = {
        Point3D(1199.99959, 299.99947, 300.00000),
        Point3D(1199.99996, 350.00010, 300.00000),
        Point3D(999.99558, 300.00240, 300.00000),
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(1199.99984, 399.99987, 400.00000),
        Point3D(1199.99971, 449.99963, 400.00000),
        Point3D(999.99558, 400.00237, 400.00000),
        Point3D(999.99583, 500.00276, 400.00000),
    };
    volumes[18]->idLithology = 17;

    volumes[19]->points = {
        Point3D(1199.99996, 350.00010, 300.00000),
        Point3D(1199.99984, 399.99987, 300.00000),
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(999.99583, 500.00276, 300.00000),
        Point3D(1199.99971, 449.99963, 400.00000),
        Point3D(1200.00009, 500.00026, 400.00000),
        Point3D(999.99583, 500.00276, 400.00000),
        Point3D(999.99558, 600.00230, 400.00000),
    };
    volumes[19]->idLithology = 24;

    volumes[20]->points = {
        Point3D(999.99558, 300.00240, 300.00000),
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(599.99547, 300.00197, 300.00000),
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(999.99558, 400.00237, 400.00000),
        Point3D(999.99583, 500.00276, 400.00000),
        Point3D(599.99567, 400.00228, 400.00000),
        Point3D(599.99569, 550.00227, 400.00000),
    };
    volumes[20]->idLithology = 1;

    volumes[21]->points = {
        Point3D(999.99558, 400.00237, 300.00000),
        Point3D(999.99583, 500.00276, 300.00000),
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(599.99572, 600.00230, 300.00000),
        Point3D(999.99583, 500.00276, 400.00000),
        Point3D(999.99558, 600.00230, 400.00000),
        Point3D(599.99569, 550.00227, 400.00000),
        Point3D(599.99547, 700.00183, 400.00000),
    };
    volumes[21]->idLithology = 3;

    volumes[22]->points = {
        Point3D(599.99547, 300.00197, 300.00000),
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(0.00001, 299.99947, 300.00000),
        Point3D(0.00001, 499.99940, 300.00000),
        Point3D(599.99567, 400.00228, 400.00000),
        Point3D(599.99569, 550.00227, 400.00000),
        Point3D(0.00026, 399.99987, 400.00000),
        Point3D(0.00026, 599.99980, 400.00000),
    };
    volumes[22]->idLithology = 5;

    volumes[23]->points = {
        Point3D(599.99569, 450.00230, 300.00000),
        Point3D(599.99572, 600.00230, 300.00000),
        Point3D(0.00001, 499.99940, 300.00000),
        Point3D(0.00001, 699.99933, 300.00000),
        Point3D(599.99569, 550.00227, 400.00000),
        Point3D(599.99547, 700.00183, 400.00000),
        Point3D(0.00026, 599.99980, 400.00000),
        Point3D(0.00021, 799.99964, 400.00000),
    };
    volumes[23]->idLithology = 7;

    return volumes;
}

RegularGrid<size_t> DomainTestValues::regularGridFromSimpleGridRotated30Degrees()
{
    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;
    const double cellSizeInX = 240;
    const double cellSizeInY = 160;
    const double cellSizeInZ = 80;
    const size_t undefinedLithology = Volume::UNDEFINED_LITHOLOGY;

    RegularGrid<size_t> regularGrid(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                cellSizeInX, cellSizeInY, cellSizeInZ,
                undefinedLithology
            );

    regularGrid.setData({
                            {
                                {4,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology},
                                {4,4,10,undefinedLithology,undefinedLithology},
                                {5,5,10,16,22},
                                {undefinedLithology,11,11,17,22},
                                {undefinedLithology,undefinedLithology,undefinedLithology,17,23}
                            },
                            {
                                {4,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology},
                                {4,4,10,undefinedLithology,undefinedLithology},
                                {5,5,10,16,22},
                                {undefinedLithology,11,11,17,22},
                                {undefinedLithology,undefinedLithology,undefinedLithology,17,23}
                            },
                            {
                                {2,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology},
                                {2,2,8,undefinedLithology,undefinedLithology},
                                {3,3,9,14,20},
                                {undefinedLithology,undefinedLithology,11,15,20},
                                {undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology,21}
                            },
                            {
                                {2,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology},
                                {2,2,8,undefinedLithology,undefinedLithology},
                                {undefinedLithology,3,9,14,20},
                                {undefinedLithology,undefinedLithology,undefinedLithology,15,20},
                                {undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology}
                            },
                            {
                                {0,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology},
                                {1,0,6,undefinedLithology,undefinedLithology},
                                {undefinedLithology,9,7,12,18},
                                {undefinedLithology,undefinedLithology,undefinedLithology,15,19},
                                {undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology,undefinedLithology}
                            }
                        });

    return regularGrid;
}

syntheticSeismic::domain::RegularGrid<double> DomainTestValues::impedanceRegularGridFromSimpleGridRotated30Degrees()
{
    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;
    const double cellSizeInX = 240;
    const double cellSizeInY = 160;
    const double cellSizeInZ = 80;

    RegularGrid<double> regularGrid(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                cellSizeInX, cellSizeInY, cellSizeInZ,
                0
            );

    regularGrid.setData({
                            {
                                {3.5, 2.5, 2.5, 2.5, 2.5},
                                {3.5, 3.5, 2.8, 2.5, 2.5},
                                {3.7, 3.7, 2.8, 4, 3.2},
                                {2.5, 3, 3, 4.5, 3.2},
                                {2.5, 2.5, 2.5, 4.5, 3.2}
                            },
                            {
                                {3.5, 2.5, 2.5, 2.5, 2.5},
                                {3.5, 3.5, 2.8, 2.5, 2.5},
                                {3.7, 3.7, 2.8, 4, 3.2},
                                {2.5, 3, 3, 4.5, 3.2},
                                {2.5, 2.5, 2.5, 4.5, 3.2}
                            },
                            {
                                {3.2, 2.5, 2.5, 2.5, 2.5},
                                {3.2, 3.2, 4.5, 2.5, 2.5},
                                {3.2, 3.2, 6, 3.5, 2.8},
                                {2.5, 2.5, 3, 3.7, 2.8},
                                {2.5, 2.5, 2.5, 2.5, 3}
                            },
                            {
                                {3.2, 2.5, 2.5, 2.5, 2.5},
                                {3.2, 3.2, 4.5, 2.5, 2.5},
                                {2.5, 3.2, 6, 3.5, 2.8},
                                {2.5, 2.5, 2.5, 3.7, 2.8},
                                {2.5, 2.5, 2.5, 2.5, 2.5}
                            },
                            {
                                {2.8, 2.5, 2.5, 2.5, 2.5},
                                {3, 2.8, 4, 2.5, 2.5},
                                {2.5, 6, 4.5, 3.2, 4.5},
                                {2.5, 2.5, 2.5, 3.7, 6},
                                {2.5, 2.5, 2.5, 2.5, 2.5}
                            }
                        });

    return regularGrid;
}

syntheticSeismic::domain::RegularGrid<double> DomainTestValues::reflectivityRegularGridFromSimpleGridRotated30Degrees()
{
    const size_t numberOfCellsInX = 5;
    const size_t numberOfCellsInY = 5;
    const size_t numberOfCellsInZ = 5;
    const double cellSizeInX = 240;
    const double cellSizeInY = 160;
    const double cellSizeInZ = 80;

    RegularGrid<double> regularGrid(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                cellSizeInX, cellSizeInY, cellSizeInZ,
                0
            );

    regularGrid.setData({
                            {
                                {-0.166667, 0.166667, 0, 0, 0},
                                {-0.166667, 0, 0.111111, 0.0566038, 0},
                                {-0.193548, 0, 0.138462, -0.176471, 0.111111},
                                {0, -0.0909091, 0, -0.2, 0.168831},
                                {0, 0, 0, -0.285714, 0.168831}
                            },
                            {
                                {-0.166667, 0.166667, 0, 0, 0},
                                {-0.166667, 0, 0.111111, 0.0566038, 0},
                                {-0.193548, 0, 0.138462, -0.176471, 0.111111},
                                {0, -0.0909091, 0, -0.2, 0.168831},
                                {0, 0, 0, -0.285714, 0.168831}
                            },
                            {
                                {-0.122807, 0.122807, 0, 0, 0},
                                {-0.122807, 0, -0.168831, 0.285714, 0},
                                {-0.122807, 0, -0.304348, 0.263158, 0.111111},
                                {0, 0, -0.0909091, -0.104478, 0.138462},
                                {0, 0, 0, 0, -0.0909091}
                            },
                            {
                                {-0.122807, 0.122807, 0, 0, 0},
                                {-0.122807, 0, -0.168831, 0.285714, 0},
                                {0, -0.122807, -0.304348, 0.263158, 0.111111},
                                {0, 0, 0, -0.193548, 0.138462},
                                {0, 0, 0, 0, 0}
                            },
                            {
                                {-0.0566038, 0.0566038, 0, 0, 0},
                                {-0.0909091, 0.0344828, -0.176471, 0.230769, 0},
                                {0, -0.411765, 0.142857, 0.168831, -0.168831},
                                {0, 0, 0, -0.193548, -0.237113},
                                {0, 0, 0, 0, 0}
                            }
                        });

    return regularGrid;
}

Wavelet DomainTestValues::rickerWavelet()
{
    Wavelet wavelet;
    wavelet.setFrequency(30.0);
    wavelet.setStep(0.468);
    wavelet.setFirst(-59.9040);
    wavelet.setLast(59.9040);
    wavelet.setUnits("ms");
    wavelet.setName(QString("Ricker wavelet (") + QString::number(wavelet.getFrequency()) + ")");
    const std::vector<double> traces = {
        -0.000000000000900249170112393783,
        -0.000000000001455113025423411032,
        -0.000000000002342525772582450862,
        -0.000000000003755992648071656551,
        -0.000000000005998143160961233968,
        -0.000000000009540248008074687857,
        -0.000000000015113053747974380338,
        -0.000000000023844788074159662772,
        -0.000000000037469880708961642320,
        -0.000000000058643199155684822069,
        -0.000000000091410980064478632943,
        -0.000000000141913358842149212238,
        -0.000000000219427573025563965049,
        -0.000000000337909885268084200039,
        -0.000000000518264076163983622048,
        -0.000000000791663328366157383591,
        -0.000000001204391892478253130189,
        -0.000000001824868673703287185013,
        -0.000000002753787908681227454512,
        -0.000000004138690805256852476902,
        -0.000000006194804333071242417810,
        -0.000000009234699627557042182135,
        -0.000000013710299044758196377925,
        -0.000000020272084554455212111506,
        -0.000000029852143532461233386424,
        -0.000000043780076204344736468855,
        -0.000000063943967375273982881738,
        -0.000000093012828487444788363540,
        -0.000000134742439338297952019838,
        -0.000000194393728758588419993909,
        -0.000000279302181961762650059844,
        -0.000000399648799743037464271100,
        -0.000000569498524727070056686924,
        -0.000000808191582189433949738838,
        -0.000001142197785002753660315807,
        -0.000001607574596698506392270693,
        -0.000002253207853218046065020899,
        -0.000003145060872217440252807032,
        -0.000004371714708601482748712326,
        -0.000006051551110128503055396714,
        -0.000008342011877581499677022751,
        -0.000011451465371916344198189967,
        -0.000015654324192029737494811364,
        -0.000021310188595730779304807806,
        -0.000028887938545890520851367952,
        -0.000038995863046039279072637468,
        -0.000052419097316983540057241248,
        -0.000070165833558270005246293444,
        -0.000093523974903297798491261239,
        -0.000124130107810269493156044018,
        -0.000164052865891371388567393508,
        -0.000215892935222300100641182330,
        -0.000282902090999566087174688089,
        -0.000369123737510147200899246478,
        -0.000479557422958402869341693053,
        -0.000620349688618468094250535394,
        -0.000799013354725453114543731736,
        -0.001024676906477330236289180476,
        -0.001308364982691805761261250041,
        -0.001663310045833756587244089964,
        -0.002105294084593250982656575943,
        -0.002653017631283116628537754877,
        -0.003328491434563882970382175941,
        -0.004157443791895956176907223067,
        -0.005169733808491344326241367924,
        -0.006399757722351856990827823068,
        -0.007886831954415621584453255366,
        -0.009675532774397114718634149710,
        -0.011815968516240990537902177948,
        -0.014363956270092324146725459855,
        -0.017381071099343604197118295929,
        -0.020934532303035525979684194908,
        -0.025096888328364447967588901633,
        -0.029945459935447876070879758004,
        -0.035561500457308291289759694109,
        -0.042029032832221167870212497064,
        -0.049433325867813231180303290557,
        -0.057858977267209170392003869665,
        -0.067387578611206436218772353186,
        -0.078094947988733634525182480957,
        -0.090047929452364908198269688455,
        -0.103300774978749190791127432476,
        -0.117891144020172367046228600884,
        -0.133835777754131551864347215997,
        -0.151125929289161714530465019379,
        -0.169722656674774236007507965951,
        -0.189552111685633351001456503582,
        -0.210500982893283894448188675597,
        -0.232412275206001162075608590385,
        -0.255081628402310800485253139414,
        -0.278254392662106719846804026020,
        -0.301623688135091938544718459525,
        -0.324829676652385268553757668997,
        -0.347460265396546397198562772246,
        -0.369053443550097060033010620828,
        -0.389101422835029520097549493585,
        -0.407056711066012488764442878164,
        -0.422340194523444623087016225327,
        -0.434351240860462284310017366806,
        -0.442479760783511477661988919863,
        -0.446120085939962829346683292897,
        -0.444686434996818702458654115617,
        -0.437629653068490553913250096230,
        -0.424454825193449181153226845709,
        -0.404739286551459576113387583973,
        -0.378150484798330077662598114330,
        -0.344463097471406132221716234199,
        -0.303574773811823461766579157484,
        -0.255519858975472857753885591592,
        -0.200480472140222409915821799586,
        -0.138794350185110437756463852566,
        -0.070958936008551687368850480198,
        0.002368715507125280107098008742,
        0.080376522398546951242437330620,
        0.162106620237712772958360574194,
        0.246469279068730368065232028130,
        0.332261338196766342623078571705,
        0.418188833084197242317969767100,
        0.502893320375727204130100744806,
        0.584981249403914937978754551295,
        0.663055588840522514004760523676,
        0.735748802392758083357193754637,
        0.801756183668139832398935595847,
        0.859868512404389773173818412033,
        0.909002985529060780756083204324,
        0.948231408589837565514812922629,
        0.976804705722041588522586152976,
        0.994172917325949367928217270673,
        1.000000000000000000000000000000,
        0.994172917325949367928217270673,
        0.976804705722041588522586152976,
        0.948231408589837565514812922629,
        0.909002985529060780756083204324,
        0.859868512404389773173818412033,
        0.801756183668139832398935595847,
        0.735748802392758083357193754637,
        0.663055588840522514004760523676,
        0.584981249403914937978754551295,
        0.502893320375727204130100744806,
        0.418188833084197242317969767100,
        0.332261338196766342623078571705,
        0.246469279068730368065232028130,
        0.162106620237712772958360574194,
        0.080376522398546951242437330620,
        0.002368715507125280107098008742,
        -0.070958936008551687368850480198,
        -0.138794350185110437756463852566,
        -0.200480472140222409915821799586,
        -0.255519858975472857753885591592,
        -0.303574773811823461766579157484,
        -0.344463097471406132221716234199,
        -0.378150484798330077662598114330,
        -0.404739286551459576113387583973,
        -0.424454825193449181153226845709,
        -0.437629653068490553913250096230,
        -0.444686434996818702458654115617,
        -0.446120085939962829346683292897,
        -0.442479760783511477661988919863,
        -0.434351240860462284310017366806,
        -0.422340194523444623087016225327,
        -0.407056711066012488764442878164,
        -0.389101422835029520097549493585,
        -0.369053443550097060033010620828,
        -0.347460265396546397198562772246,
        -0.324829676652385268553757668997,
        -0.301623688135091938544718459525,
        -0.278254392662106719846804026020,
        -0.255081628402310800485253139414,
        -0.232412275206001162075608590385,
        -0.210500982893283894448188675597,
        -0.189552111685633351001456503582,
        -0.169722656674774236007507965951,
        -0.151125929289161714530465019379,
        -0.133835777754131551864347215997,
        -0.117891144020172367046228600884,
        -0.103300774978749190791127432476,
        -0.090047929452364908198269688455,
        -0.078094947988733634525182480957,
        -0.067387578611206436218772353186,
        -0.057858977267209170392003869665,
        -0.049433325867813231180303290557,
        -0.042029032832221167870212497064,
        -0.035561500457308291289759694109,
        -0.029945459935447876070879758004,
        -0.025096888328364447967588901633,
        -0.020934532303035525979684194908,
        -0.017381071099343604197118295929,
        -0.014363956270092324146725459855,
        -0.011815968516240990537902177948,
        -0.009675532774397114718634149710,
        -0.007886831954415621584453255366,
        -0.006399757722351856990827823068,
        -0.005169733808491344326241367924,
        -0.004157443791895956176907223067,
        -0.003328491434563882970382175941,
        -0.002653017631283116628537754877,
        -0.002105294084593250982656575943,
        -0.001663310045833756587244089964,
        -0.001308364982691805761261250041,
        -0.001024676906477330236289180476,
        -0.000799013354725453114543731736,
        -0.000620349688618468094250535394,
        -0.000479557422958402869341693053,
        -0.000369123737510147200899246478,
        -0.000282902090999566087174688089,
        -0.000215892935222300100641182330,
        -0.000164052865891371388567393508,
        -0.000124130107810269493156044018,
        -0.000093523974903297798491261239,
        -0.000070165833558270005246293444,
        -0.000052419097316983540057241248,
        -0.000038995863046039279072637468,
        -0.000028887938545890520851367952,
        -0.000021310188595730779304807806,
        -0.000015654324192029737494811364,
        -0.000011451465371916344198189967,
        -0.000008342011877581499677022751,
        -0.000006051551110128503055396714,
        -0.000004371714708601482748712326,
        -0.000003145060872217440252807032,
        -0.000002253207853218046065020899,
        -0.000001607574596698506392270693,
        -0.000001142197785002753660315807,
        -0.000000808191582189433949738838,
        -0.000000569498524727070056686924,
        -0.000000399648799743037464271100,
        -0.000000279302181961762650059844,
        -0.000000194393728758588419993909,
        -0.000000134742439338297952019838,
        -0.000000093012828487444788363540,
        -0.000000063943967375273982881738,
        -0.000000043780076204344736468855,
        -0.000000029852143532461233386424,
        -0.000000020272084554455212111506,
        -0.000000013710299044758196377925,
        -0.000000009234699627557042182135,
        -0.000000006194804333071242417810,
        -0.000000004138690805256852476902,
        -0.000000002753787908681227454512,
        -0.000000001824868673703287185013,
        -0.000000001204391892478253130189,
        -0.000000000791663328366157383591,
        -0.000000000518264076163983622048,
        -0.000000000337909885268084200039,
        -0.000000000219427573025563965049,
        -0.000000000141913358842149212238,
        -0.000000000091410980064478632943,
        -0.000000000058643199155684822069,
        -0.000000000037469880708961642320,
        -0.000000000023844788074159662772,
        -0.000000000015113053747974380338,
        -0.000000000009540248008074687857,
        -0.000000000005998143160961233968,
        -0.000000000003755992648071656551,
        -0.000000000002342525772582450862,
        -0.000000000001455113025423411032,
        -0.000000000000900249170112393783
    };
    wavelet.setTraces(traces);

    return wavelet;
}

Wavelet DomainTestValues::waveletToTestConvolution()
{
    Wavelet wavelet;
    wavelet.setFrequency(30.0);
    wavelet.setStep(0.468);
    wavelet.setFirst(-59.9040);
    wavelet.setLast(59.9040);
    wavelet.setUnits("ms");
    wavelet.setName(QString("Ricker wavelet (") + QString::number(wavelet.getFrequency()) + ")");
    const std::vector<double> traces = {0.6, 1.9, -2.2};
    wavelet.setTraces(traces);

    return wavelet;
}

syntheticSeismic::domain::RegularGrid<double> DomainTestValues::regularGridToTestConvolution()
{
    const size_t numberOfCellsInX = 1;
    const size_t numberOfCellsInY = 1;
    const size_t numberOfCellsInZ = 5;
    const double cellSizeInX = 1;
    const double cellSizeInY = 1;
    const double cellSizeInZ = 1;

    RegularGrid<double> regularGrid(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                cellSizeInX, cellSizeInY, cellSizeInZ,
                0.0, 0
            );

    regularGrid.setData({{{0.2, 0.5, -0.4, 1.0, 0.0}}});

    return regularGrid;
}

syntheticSeismic::domain::RegularGrid<double> DomainTestValues::regularGridConvolution()
{
    const size_t numberOfCellsInX = 1;
    const size_t numberOfCellsInY = 1;
    const size_t numberOfCellsInZ = 7;
    const double cellSizeInX = 1;
    const double cellSizeInY = 1;
    const double cellSizeInZ = 1;

    RegularGrid<double> regularGrid(
                numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
                cellSizeInX, cellSizeInY, cellSizeInZ,
                0.0, 0
            );

    regularGrid.setData({{{0.12, 0.68, 0.27, -1.26, 2.78, -2.2, 0}}});

    return regularGrid;
}
