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

    template<typename ShiftFunction>
    Coord executeMovement(Direction direction, Coord robotPosition, Array2D<char> &warehouse, ShiftFunction shift) {
        auto adjacentPosition = robotPosition + getDirectionIncrements(direction);
        bool shiftSuccessful = shift(robotPosition, adjacentPosition, warehouse, direction);
        if (shiftSuccessful) {
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

}

namespace part1 {

    bool shift(Coord current, Coord adjacent, Array2D<char> &warehouse, Direction direction) {
        // Recursive.
        // end condition: place == ".", return true, place == '#', return false
        // All children must return true in order to execute the shift.
        char adjacentType = warehouse(adjacent.row, adjacent.col);
        if (adjacentType == '.') {
            return true;
        } else if (adjacentType == '#') {
            return false;
        } else if (adjacentType == 'O'){
            auto nextAdjacent = adjacent + getDirectionIncrements(direction);
            bool shiftSuccessful = shift(adjacent, nextAdjacent, warehouse, direction);
            if (shiftSuccessful) {
                warehouse(nextAdjacent.row, nextAdjacent.col) = warehouse(adjacent.row, adjacent.col);
                return true;
            } else {
                return false;
            }
        }
        else {
            // Shift robot too
            warehouse(adjacent.row, adjacent.col) = warehouse(current.row, current.col);
            warehouse(current.row, current.col) = '.';
            return true;
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
        Coord robotPosition = *robotCoord;

        for (const auto &movement: input.movements) {
            auto direction = directionFromMovementChar(movement);
            robotPosition = executeMovement(direction, robotPosition, warehouse, shift);
        }

        auto gps = computeGps(warehouse);
        std::cout << gps << std::endl;
    }
}

namespace part2 {

    Array2D<char> enlargeWarehouse(const Array2D<char> &warehouse) {
        return warehouse;
    }

    bool canShift(Coord adjacent, Array2D<char> &warehouse, Direction direction) {
        char adjacentType = warehouse(adjacent.row, adjacent.col);
        if (adjacentType == '.') {
            return true;
        } else if (adjacentType == '#') {
            return false;
        }

        if (direction == Direction::Up || direction == Direction::Down) {
            if (adjacentType == '[') {
                auto adjacentRight = adjacent + getDirectionIncrements(Direction::Right);
                auto nextLeft = adjacent + getDirectionIncrements(direction);
                auto nextRight = adjacentRight + getDirectionIncrements(direction);

                return canShift(nextLeft, warehouse, direction) &&
                       canShift(nextRight, warehouse, direction);
            } else { // ']'
                auto adjacentLeft = adjacent + getDirectionIncrements(Direction::Left);
                auto nextLeft = adjacentLeft + getDirectionIncrements(direction);
                auto nextRight = adjacent + getDirectionIncrements(direction);

                return canShift(nextLeft, warehouse, direction) &&
                       canShift(nextRight, warehouse, direction);
            }
        } else {
            auto nextAdjacent = adjacent + getDirectionIncrements(direction);
            return canShift(nextAdjacent, warehouse, direction);
        }
    }

    void shift(Coord adjacent, Array2D<char> &warehouse, Direction direction) {
        char adjacentType = warehouse(adjacent.row, adjacent.col);
        if (adjacentType == '.' || adjacentType == '#') {
            return;
        }
        else if (adjacentType == '[' || adjacentType == ']') {
            if (direction == Direction::Up || direction == Direction::Down) {
                if (adjacentType == '[') {
                    auto adjacentRight = adjacent + getDirectionIncrements(Direction::Right);
                    auto nextLeft = adjacent + getDirectionIncrements(direction);
                    auto nextRight = adjacentRight + getDirectionIncrements(direction);

                    shift(nextLeft, warehouse, direction);
                    shift(nextRight, warehouse, direction);

                    warehouse(nextLeft.row, nextLeft.col) = warehouse(adjacent.row, adjacent.col);
                    warehouse(nextRight.row, nextRight.col) = warehouse(adjacentRight.row, adjacentRight.col);
                } else { // ']'
                    auto adjacentLeft = adjacent + getDirectionIncrements(Direction::Left);
                    auto nextLeft = adjacentLeft + getDirectionIncrements(direction);
                    auto nextRight = adjacent + getDirectionIncrements(direction);

                    shift(nextLeft, warehouse, direction);
                    shift(nextRight, warehouse, direction);

                    warehouse(nextLeft.row, nextLeft.col) = warehouse(adjacentLeft.row, adjacentLeft.col);
                    warehouse(nextRight.row, nextRight.col) = warehouse(adjacent.row, adjacent.col);
                }
            } else {
                auto nextAdjacent = adjacent + getDirectionIncrements(direction);
                shift(nextAdjacent, warehouse, direction);

                warehouse(nextAdjacent.row, nextAdjacent.col) = warehouse(adjacent.row, adjacent.col);
            }
        }
    }

    uint64_t computeGps(const Array2D<char> &warehouse) {
        uint64_t gpsSum = 0;

        forEachItemWithCoords(warehouse, '[', [&gpsSum](const Coord &coord) {
            gpsSum += (coord.row * 100 + coord.col);
        });
        return gpsSum;
    }


    bool shiftIfPossible(Coord current, Coord adjacent, Array2D<char> &warehouse, Direction direction) {
        auto shiftIsPossible = canShift(adjacent, warehouse, direction);
        if (shiftIsPossible) {
            shift(adjacent, warehouse, direction);
        }
        return shiftIsPossible;
    }

    template<typename Input>
    void execute(const Input &input) {
        auto warehouse = enlargeWarehouse(input.warehouse);

        auto robotCoord = findRobot(warehouse);
        Coord robotPosition = *robotCoord;

        for (const auto &movement: input.movements) {
            std::cout << warehouse << std::endl;

            auto direction = directionFromMovementChar(movement);
            robotPosition = executeMovement(direction, robotPosition, warehouse, shiftIfPossible);
        }

        std::cout << warehouse << std::endl;

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
