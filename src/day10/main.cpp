#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "input.h"
#include "array2d.h"
#include "Coord.h"
#include "timer.h"


namespace {

    using Map = Array2D<int>;

    int transformInputIntoIntegers(char character) {
        if (character == '.') {
            return -1;
        }
        return character - '0';
    }

    Map loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        auto map = input::load2D<int>(lines, transformInputIntoIntegers);
        return map;
    }

}

namespace part1 {

    void stepUntilFinalPosition(int previousValue, Coord currentCoord, const Map &topographicMap, std::unordered_set<Coord> &finalCoords) {
        if (!topographicMap.isInBounds(currentCoord.row, currentCoord.col)) {
            return;
        }
        int currentValue = topographicMap(currentCoord.row, currentCoord.col);
        if (currentValue != previousValue + 1) {
            return;
        }
        if (currentValue == 9) {
            finalCoords.insert(currentCoord);
        }
        else {
            stepUntilFinalPosition(currentValue, currentCoord.getUpCoord(), topographicMap, finalCoords);
            stepUntilFinalPosition(currentValue, currentCoord.getDownCoord(), topographicMap, finalCoords);
            stepUntilFinalPosition(currentValue, currentCoord.getLeftCoord(), topographicMap, finalCoords);
            stepUntilFinalPosition(currentValue, currentCoord.getRightCoord(), topographicMap, finalCoords);
        }
    }

    template<typename Input>
    void execute(Input &input) {
        const auto &topographicMap = input;
        auto rows = topographicMap.rows();
        auto cols = topographicMap.cols();

        int score = 0;
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                // If starting position
                if (topographicMap(row, col) == 0) {
                    std::unordered_set<Coord> finalCoords{};
                    auto currentCoord = Coord{.col=col,.row=row};
                    stepUntilFinalPosition(0, currentCoord.getUpCoord(), topographicMap, finalCoords);
                    stepUntilFinalPosition(0, currentCoord.getDownCoord(), topographicMap, finalCoords);
                    stepUntilFinalPosition(0, currentCoord.getLeftCoord(), topographicMap, finalCoords);
                    stepUntilFinalPosition(0, currentCoord.getRightCoord(), topographicMap, finalCoords);
                    score += static_cast<int>(finalCoords.size());
                }
            }
        }
        std::cout << score << std::endl;
    }
}

namespace part2 {

    void increaseCountIfValid(int targetValue, Coord targetCoord, Coord currentCoord, const Map &topographicalMap,
                              Map &neighborCounts, Map &currentCounts) {
        if (!topographicalMap.isInBounds(targetCoord.row, targetCoord.col)) {
            return;
        }
        int actualValue = topographicalMap(targetCoord.row, targetCoord.col);
        if (actualValue != targetValue) {
            return;
        }
        neighborCounts(targetCoord.row, targetCoord.col) += currentCounts(currentCoord.row, currentCoord.col);
    }

    int countTrails(const Map &topographicMap, const Map &zeroNumberCounts) {
        int trailCount = 0;
        for (int row = 0; row < topographicMap.rows(); row++) {
            for (int col = 0; col < topographicMap.cols(); col++) {
                // If we are at the number of interest
                if (topographicMap(row, col) == 0) {
                    trailCount += zeroNumberCounts(row, col);
                }
            }
        }
        return trailCount;
    }

    int countDistinctTrails(const Map &topographicMap) {
        auto rows = topographicMap.rows();
        auto cols = topographicMap.cols();
        // Initialize the counts of neighbors that are smaller by 1
        std::array<Map, 10> smallerNeighborCounts;
        for (int dir = 0; dir < 10; ++dir) {
            smallerNeighborCounts[dir] = Map(topographicMap.rows(), topographicMap.cols());
        }
        // Set all values of 1 to the first map to simplify the algorithm.
        std::fill(smallerNeighborCounts[0].begin(), smallerNeighborCounts[0].end(), 1);

        for (int i = 0; i < 9; i++) {
            int currentValue = 9 - i;
            int smallerNeighborValue = currentValue - 1;
            auto &currentCounts = smallerNeighborCounts[i];
            auto &neighborCounts = smallerNeighborCounts[i + 1];

            for (int row = 0; row < rows; row++) {
                for (int col = 0; col < cols; col++) {
                    // If we are at the number of interest
                    if (topographicMap(row, col) == currentValue) {
                        // We look at its neighbors and increase the count for their location if they are smaller by 1
                        auto coord = Coord{.col=col, .row=row};
                        increaseCountIfValid(smallerNeighborValue, coord.getUpCoord(), coord, topographicMap,
                                             neighborCounts, currentCounts);
                        increaseCountIfValid(smallerNeighborValue, coord.getDownCoord(), coord, topographicMap,
                                             neighborCounts, currentCounts);
                        increaseCountIfValid(smallerNeighborValue, coord.getLeftCoord(), coord, topographicMap,
                                             neighborCounts, currentCounts);
                        increaseCountIfValid(smallerNeighborValue, coord.getRightCoord(), coord, topographicMap,
                                             neighborCounts, currentCounts);
                    }

                }
            }
        }

        int trailCount = countTrails(topographicMap, smallerNeighborCounts[9]);
        return trailCount;
    }

    template<typename Input>
    void execute(Input &input) {
        int trailCount = countDistinctTrails(input);
        std::cout << trailCount << std::endl;
    }
}

int main() {
    auto input = loadInput("day10.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
