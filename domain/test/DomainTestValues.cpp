#include "DomainTestValues.h"
#include <geometry/src/Coordinate.h>

using namespace syntheticSeismic::geometry;
using namespace syntheticSeismic::domain;

EclipseGrid DomainTestValues::eclipseGridFromSimpleGrid()
{
    const size_t numberOfCellsInX = 2;
    const size_t numberOfCellsInY = 3;
    const size_t numberOfCellsInZ = 4;

    std::vector<Coordinate> coordinates = {
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

    std::vector<double> zCoordinates = {
        1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
        1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
        1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
        1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
        1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200, 1200,
        1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300,
        1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300,
        1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1400
    };

    std::vector<int> lithologyIds = {
         1,  2,  3,  4,  5,  6,
         7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24
    };

    return EclipseGrid(numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ, coordinates, zCoordinates, lithologyIds);
}

std::vector<Volume> DomainTestValues::volumesOfFirstLayerFromSimpleGrid()
{
    std::vector<Volume> volumesCompare(6);
    volumesCompare[0].m_points = {
        Point3D(1000, 2000, 1000),
        Point3D(1050, 2000, 1000),
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1100, 2000, 1100),
        Point3D(1150, 2000, 1100),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
    };
    volumesCompare[1].m_points = {
        Point3D(1050, 2000, 1000),
        Point3D(1100, 2000, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2000, 1100),
        Point3D(1200, 2000, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
    };
    volumesCompare[2].m_points = {
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1100, 2200, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
    };
    volumesCompare[3].m_points = {
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
    };
    volumesCompare[4].m_points = {
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1000, 3200, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1100, 2600, 1100),
        Point3D(1250, 2600, 1100),
        Point3D(1100, 3200, 1100),
        Point3D(1300, 3200, 1100),
    };
    volumesCompare[5].m_points = {
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1400, 3200, 1000),
        Point3D(1250, 2600, 1100),
        Point3D(1400, 2600, 1100),
        Point3D(1300, 3200, 1100),
        Point3D(1500, 3200, 1100),
    };

    return volumesCompare;
}

std::vector<Volume> DomainTestValues::volumesFromSimpleGrid()
{
    std::vector<Volume> volumesCompare(24);
    volumesCompare[0].m_points = {
        Point3D(1000, 2000, 1000),
        Point3D(1050, 2000, 1000),
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2000, 1100),
        Point3D(1250, 2000, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
    };
    volumesCompare[1].m_points = {
        Point3D(1050, 2000, 1000),
        Point3D(1100, 2000, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1250, 2000, 1100),
        Point3D(1300, 2000, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1400, 2200, 1100),
    };
    volumesCompare[2].m_points = {
        Point3D(1000, 2200, 1000),
        Point3D(1100, 2200, 1000),
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1200, 2600, 1100),
        Point3D(1350, 2600, 1100),
    };
    volumesCompare[3].m_points = {
        Point3D(1100, 2200, 1000),
        Point3D(1200, 2200, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1300, 2200, 1100),
        Point3D(1400, 2200, 1100),
        Point3D(1350, 2600, 1100),
        Point3D(1500, 2600, 1100),
    };
    volumesCompare[4].m_points = {
        Point3D(1000, 2600, 1000),
        Point3D(1150, 2600, 1000),
        Point3D(1000, 3200, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1200, 2600, 1100),
        Point3D(1350, 2600, 1100),
        Point3D(1200, 3200, 1100),
        Point3D(1400, 3200, 1100),
    };
    volumesCompare[5].m_points = {
        Point3D(1150, 2600, 1000),
        Point3D(1300, 2600, 1000),
        Point3D(1200, 3200, 1000),
        Point3D(1400, 3200, 1000),
        Point3D(1350, 2600, 1100),
        Point3D(1500, 2600, 1100),
        Point3D(1400, 3200, 1100),
        Point3D(1600, 3200, 1100),
    };
    volumesCompare[6].m_points = {
        Point3D(1200, 2000, 1100),
        Point3D(1250, 2000, 1100),
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1400, 2000, 1200),
        Point3D(1450, 2000, 1200),
        Point3D(1400, 2200, 1200),
        Point3D(1500, 2200, 1200),
    };
    volumesCompare[7].m_points = {
        Point3D(1250, 2000, 1100),
        Point3D(1300, 2000, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1400, 2200, 1100),
        Point3D(1450, 2000, 1200),
        Point3D(1500, 2000, 1200),
        Point3D(1500, 2200, 1200),
        Point3D(1600, 2200, 1200),
    };
    volumesCompare[8].m_points = {
        Point3D(1200, 2200, 1100),
        Point3D(1300, 2200, 1100),
        Point3D(1200, 2600, 1100),
        Point3D(1350, 2600, 1100),
        Point3D(1400, 2200, 1200),
        Point3D(1500, 2200, 1200),
        Point3D(1400, 2600, 1200),
        Point3D(1550, 2600, 1200),
    };
    volumesCompare[9].m_points = {
        Point3D(1300, 2200, 1100),
        Point3D(1400, 2200, 1100),
        Point3D(1350, 2600, 1100),
        Point3D(1500, 2600, 1100),
        Point3D(1500, 2200, 1200),
        Point3D(1600, 2200, 1200),
        Point3D(1550, 2600, 1200),
        Point3D(1700, 2600, 1200),
    };
    volumesCompare[10].m_points = {
        Point3D(1200, 2600, 1100),
        Point3D(1350, 2600, 1100),
        Point3D(1200, 3200, 1100),
        Point3D(1400, 3200, 1100),
        Point3D(1400, 2600, 1200),
        Point3D(1550, 2600, 1200),
        Point3D(1400, 3200, 1200),
        Point3D(1600, 3200, 1200),
    };
    volumesCompare[11].m_points = {
        Point3D(1350, 2600, 1100),
        Point3D(1500, 2600, 1100),
        Point3D(1400, 3200, 1100),
        Point3D(1600, 3200, 1100),
        Point3D(1550, 2600, 1200),
        Point3D(1700, 2600, 1200),
        Point3D(1600, 3200, 1200),
        Point3D(1800, 3200, 1200),
    };
    volumesCompare[12].m_points = {
        Point3D(1400, 2000, 1200),
        Point3D(1450, 2000, 1200),
        Point3D(1400, 2200, 1200),
        Point3D(1500, 2200, 1200),
        Point3D(1600, 2000, 1300),
        Point3D(1650, 2000, 1300),
        Point3D(1600, 2200, 1300),
        Point3D(1700, 2200, 1300),
    };
    volumesCompare[13].m_points = {
        Point3D(1450, 2000, 1200),
        Point3D(1500, 2000, 1200),
        Point3D(1500, 2200, 1200),
        Point3D(1600, 2200, 1200),
        Point3D(1650, 2000, 1300),
        Point3D(1700, 2000, 1300),
        Point3D(1700, 2200, 1300),
        Point3D(1800, 2200, 1300),
    };
    volumesCompare[14].m_points = {
        Point3D(1400, 2200, 1200),
        Point3D(1500, 2200, 1200),
        Point3D(1400, 2600, 1200),
        Point3D(1550, 2600, 1200),
        Point3D(1600, 2200, 1300),
        Point3D(1700, 2200, 1300),
        Point3D(1600, 2600, 1300),
        Point3D(1750, 2600, 1300),
    };
    volumesCompare[15].m_points = {
        Point3D(1500, 2200, 1200),
        Point3D(1600, 2200, 1200),
        Point3D(1550, 2600, 1200),
        Point3D(1700, 2600, 1200),
        Point3D(1700, 2200, 1300),
        Point3D(1800, 2200, 1300),
        Point3D(1750, 2600, 1300),
        Point3D(1900, 2600, 1300),
    };
    volumesCompare[16].m_points = {
        Point3D(1400, 2600, 1200),
        Point3D(1550, 2600, 1200),
        Point3D(1400, 3200, 1200),
        Point3D(1600, 3200, 1200),
        Point3D(1600, 2600, 1300),
        Point3D(1750, 2600, 1300),
        Point3D(1600, 3200, 1300),
        Point3D(1800, 3200, 1300),
    };
    volumesCompare[17].m_points = {
        Point3D(1550, 2600, 1200),
        Point3D(1700, 2600, 1200),
        Point3D(1600, 3200, 1200),
        Point3D(1800, 3200, 1200),
        Point3D(1750, 2600, 1300),
        Point3D(1900, 2600, 1300),
        Point3D(1800, 3200, 1300),
        Point3D(2000, 3200, 1300),
    };
    volumesCompare[18].m_points = {
        Point3D(1600, 2000, 1300),
        Point3D(1650, 2000, 1300),
        Point3D(1600, 2200, 1300),
        Point3D(1700, 2200, 1300),
        Point3D(1800, 2000, 1400),
        Point3D(1850, 2000, 1400),
        Point3D(1800, 2200, 1400),
        Point3D(1900, 2200, 1400),
    };
    volumesCompare[19].m_points = {
        Point3D(1650, 2000, 1300),
        Point3D(1700, 2000, 1300),
        Point3D(1700, 2200, 1300),
        Point3D(1800, 2200, 1300),
        Point3D(1850, 2000, 1400),
        Point3D(1900, 2000, 1400),
        Point3D(1900, 2200, 1400),
        Point3D(2000, 2200, 1400),
    };
    volumesCompare[20].m_points = {
        Point3D(1600, 2200, 1300),
        Point3D(1700, 2200, 1300),
        Point3D(1600, 2600, 1300),
        Point3D(1750, 2600, 1300),
        Point3D(1800, 2200, 1400),
        Point3D(1900, 2200, 1400),
        Point3D(1800, 2600, 1400),
        Point3D(1950, 2600, 1400),
    };
    volumesCompare[21].m_points = {
        Point3D(1700, 2200, 1300),
        Point3D(1800, 2200, 1300),
        Point3D(1750, 2600, 1300),
        Point3D(1900, 2600, 1300),
        Point3D(1900, 2200, 1400),
        Point3D(2000, 2200, 1400),
        Point3D(1950, 2600, 1400),
        Point3D(2100, 2600, 1400),
    };
    volumesCompare[22].m_points = {
        Point3D(1600, 2600, 1300),
        Point3D(1750, 2600, 1300),
        Point3D(1600, 3200, 1300),
        Point3D(1800, 3200, 1300),
        Point3D(1800, 2600, 1400),
        Point3D(1950, 2600, 1400),
        Point3D(1800, 3200, 1400),
        Point3D(2000, 3200, 1400),
    };
    volumesCompare[23].m_points = {
        Point3D(1750, 2600, 1300),
        Point3D(1900, 2600, 1300),
        Point3D(1800, 3200, 1300),
        Point3D(2000, 3200, 1300),
        Point3D(1950, 2600, 1400),
        Point3D(2100, 2600, 1400),
        Point3D(2000, 3200, 1400),
        Point3D(2200, 3200, 1400),
    };

    return volumesCompare;
}

std::vector<Point2D> DomainTestValues::minimumRectangleFromSimpleGrid()
{
    return {
        {2200, 2000},
        {2200, 3200},
        {1000, 3200},
        {1000, 2000}
    };
}
