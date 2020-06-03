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
