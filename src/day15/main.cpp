#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <optional>
#include "Coord.h"
#include "input.h"
#include "print.h"
#include "array2d.h"
#include "timer.h"
#include "Direction.h"


namespace {

    struct Input {
        Array2D<char> warehouse;
        std::string movements;
    };

    Input loadInput(const std::string &filename) {
        auto content = input::readFile<std::string>(filename, input::read);
        auto mainParts = input::split(content, "\n\n", input::Blanks::Remove);
        auto warehouseLines = input::split(mainParts[0], '\n');
        auto warehouse = input::load2D<char>(warehouseLines, [](char character) { return character; });
        auto movementsLines = input::split(mainParts[1], '\n');
        auto movements = input::join(movementsLines);
        return {warehouse, movements};
    }

}

namespace part1 {


    template<typename Input>
    void execute(const Input &input) {
        std::cout << input.warehouse << std::endl;


    }
}

namespace part2 {

    std::optional<Coord> findRobot(const Array2D<char> &warehouse) {
        auto it = std::find(warehouse.cbegin(), warehouse.cend(), '@');
        if (it != warehouse.cend()) {
            size_t flatIndex = std::distance(warehouse.cbegin(), it);
            auto coord = warehouse.toIndex2D(static_cast<int>(flatIndex));
            return coord;
        }
        return std::nullopt;
    }

    Coord getDirectionIncrements(Direction direction) {
        int rowIncrement = 0;
        int colIncrement = 0;

        switch (direction) {
            case Direction::Right:
                colIncrement = 1;
                break;
            case Direction::Left:
                colIncrement = -1;
                break;
            case Direction::Up:
                rowIncrement = -1;
                break;
            case Direction::Down:
                rowIncrement = 1;
                break;
        }
        return Coord{.col=colIncrement, .row=rowIncrement};
    }

    Direction directionFromMovementChar(char movement) {
        if (movement == '>') {
            return Direction::Right;
        } else if (movement == '<') {
            return Direction::Left;
        } else if (movement == '^') {
            return Direction::Up;
        } else if (movement == 'v') {
            return Direction::Down;
        } else {
            throw std::runtime_error("Unrecognized movement symbol.");
        }
    }

    bool shift(Coord current, Coord adjacent, Array2D<char> &warehouse, Direction direction) {
        // Recursive.
        // end condition: place == ".", return true, place == '#', return false
        // All children must return true in order to execute the shift.
        char adjacentType = warehouse(adjacent.row, adjacent.col);
        if (adjacentType == '.') {
            return true;
        } else if (adjacentType == '#') {
            return false;
        } else {
            auto nextAdjacent = adjacent + getDirectionIncrements(direction);
            bool shiftSuccessful = shift(adjacent, nextAdjacent, warehouse, direction);
            if (shiftSuccessful) {
                warehouse(nextAdjacent.row, nextAdjacent.col) = warehouse(adjacent.row, adjacent.col);
                return true;
            }
            else {
                return false;
            }
        }
    }

    Coord executeMovement(Direction direction, Coord robotPosition, Array2D<char> &warehouse) {
        auto adjacentPosition = robotPosition + getDirectionIncrements(direction);
        bool shiftSuccessful = shift(robotPosition, adjacentPosition, warehouse, direction);
        if (shiftSuccessful) {
            // Shift robot too
            warehouse(adjacentPosition.row, adjacentPosition.col) = warehouse(robotPosition.row, robotPosition.col);
            warehouse(robotPosition.row, robotPosition.col) = '.';

            return adjacentPosition;
        } else {
            return robotPosition;
        }
    }

    template<typename T, typename UnaryFunction>
    void forEachItemWithCoords(const Array2D<T> &array, T target, UnaryFunction action) {
        auto it = array.cbegin();
        auto end = array.cend();
        while (it != end) {
            if (*it == target) {
                size_t flatIndex = std::distance(array.cbegin(), it);
                Coord coord = array.toIndex2D(flatIndex);
                action(coord);
            }
            ++it;
        }
    }

    uint64_t computeGps(const Array2D<char> &warehouse) {
        uint64_t gpsSum = 0;

        forEachItemWithCoords(warehouse, 'O', [&gpsSum](const Coord &coord) {
            gpsSum += (coord.row * 100 + coord.col);
        });
        return gpsSum;
    }

    template<typename Input>
    void execute(const Input &input) {
        auto warehouse = input.warehouse;

        auto robotCoord = findRobot(warehouse);
        std::cout << *robotCoord << std::endl;
        Coord robotPosition = *robotCoord;

        for (const auto &movement: input.movements) {
            auto direction = directionFromMovementChar(movement);
            robotPosition = executeMovement(direction, robotPosition, warehouse);
        }

        auto gps = computeGps(warehouse);
        std::cout << gps << std::endl;
    }
}

int main() {
    auto input = loadInput("day15.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
