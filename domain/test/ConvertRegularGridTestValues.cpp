#include <iostream>
#include "ConvertRegularGridTestValues.h"

using namespace syntheticSeismic::geometry;
using namespace syntheticSeismic::domain;

RegularGrid<std::shared_ptr<Volume>> ConvertRegularGridTestValues::regularGridLithologyOneDimensionalDepth() {
    const size_t numberOfCellsInX = 1;
    const size_t numberOfCellsInY = 1;
    const size_t numberOfCellsInZ = 1252;
    const auto cellSizeInX = 1.0;
    const auto cellSizeInY = 1.0;
    const auto cellSizeInZ = 1.0;
    const auto unitInX = EnumUnit::Meters;
    const auto unitInY = EnumUnit::Meters;
    const auto unitInZ = EnumUnit::Meters;
    const auto rectanglePoints = std::array<Point2D, 4>();
    const auto zBottom = 0.0;
    const auto zTop = 0.0;
    const std::shared_ptr<Volume> defaultValue = nullptr;

    RegularGrid<std::shared_ptr<Volume>> regularGrid(
            numberOfCellsInX, numberOfCellsInY, numberOfCellsInZ,
            cellSizeInX, cellSizeInY, cellSizeInZ,
            unitInX, unitInY, unitInZ,
            rectanglePoints,
            zBottom,
            zTop,
            defaultValue
    );
    auto &data = regularGrid.getData();

    auto pairs = std::vector<std::vector<int>> {
            {-1, 68}, {2, 30}, {1, 14}, {2, 9}, {1, 5},  {2, 38}, {4, 5}, {2, 62}, {1, 5}, {2, 14}, {1, 15}, {2, 9},
            {1, 19},  {2, 54}, {1, 22}, {2, 8}, {3, 31}, {2, 5},  {1, 8}, {2, 14}, {1, 4}, {2, 4},  {1, 31}, {2, 4},
            {4, 9},   {2, 22}, {1, 9},  {2, 4}, {4, 48}, {3, 14}, {4, 4}, {3, 4}, {4, 13}, {2, 5},  {1, 9},  {4, 8},
            {1, 7},   {4, 12}, {1, 4}, {2, 14}, {1, 3},  {2, 8}, {4, 10}, {1, 4}, {4, 10}, {3, 7},  {4, 15}, {2, 3},
            {3, 7},   {5, 4},  {1, 3}, {2, 11}, {1, 21}, {4, 7}, {6, 14}, {2, 4}, {6, 11}, {3, 3},  {2, 7}, {3, 34},
            {1, 232}, {2, 5}, {1, 48}, {2, 5},  {1, 11}, {8, 26}, {3, 27}, {-1, 48}
    };
    auto pairsSize = pairs.size();

    auto lithologies = std::vector<int>();
    long long count = 0;
    for (size_t i = 0; i < pairsSize; ++i) {
        const auto pair = pairs[i];

        if (pair[0] == -1) {
            std::fill(data[0][0].begin() + count, data[0][0].begin() + count + pair[1], nullptr);

            count += pair[1];

            continue;
        }

        auto volume = std::make_shared<Volume>(i);
        volume->idLithology = pair[0];

        std::fill(data[0][0].begin() + count, data[0][0].begin() + count + pair[1], volume);

        count += pair[1];
    }

    return regularGrid;
}

std::vector<int> ConvertRegularGridTestValues::lithologiesInTime() {
    std::vector<int> data(2522);

    auto pairs = std::vector<std::vector<int>> {
            {-1, 163}, {2, 60}, {1, 30}, {2, 18}, {1, 11}, {2, 76}, {4, 8}, {2, 124}, {1, 11}, {2, 28}, {1, 32},
            {2, 18}, {1, 41}, {2, 108}, {1, 47}, {2, 16}, {3, 58}, {2, 10}, {1, 17}, {2, 28}, {1, 9}, {2, 8},
            {1, 66}, {2, 8}, {4, 16}, {2, 44}, {1, 19}, {2, 8}, {4, 82}, {3, 27}, {4, 7}, {3, 7}, {4, 22},
            {2, 10}, {1, 20}, {4, 13}, {1, 15}, {4, 21}, {1, 9}, {2, 28}, {1, 6}, {2, 16}, {4, 17}, {1, 9},
            {4, 17}, {3, 13}, {4, 26}, {2, 6}, {3, 13}, {5, 6}, {1, 7}, {2, 22}, {1, 45}, {4, 12}, {6, 21},
            {2, 8}, {6, 16}, {3, 6}, {2, 14}, {3, 64}, {1, 497}, {2, 10}, {1, 103}, {2, 10}, {1, 23}, {8, 26},
            {3, 51}, {-1, 115}
    };
    auto pairsSize = pairs.size();

    auto lithologies = std::vector<int>();
    long long count = 0;
    for (size_t i = 0; i < pairsSize; ++i) {
        const auto pair = pairs[i];

        std::fill(data.begin() + count, data.begin() + count + pair[1], pair[0]);

        count += pair[1];
    }

    return data;
}
