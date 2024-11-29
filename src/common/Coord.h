//
// Created by lada on 10/12/24.
//

#ifndef AOC_2023_COORD_H
#define AOC_2023_COORD_H

#include <array>

struct Coord {
    int col;
    int row;

    bool operator==(const Coord &other) const {
        return col == other.col && row == other.row;
    }

    bool operator!=(const Coord &other) const {
        return !this->operator==(other);
    }

    [[nodiscard]] Coord getUpCoord() const {
        return {col, row - 1};
    }

    [[nodiscard]] Coord getDownCoord() const {
        return {col, row + 1};
    }

    [[nodiscard]] Coord getLeftCoord() const {
        return {col - 1, row};
    }

    [[nodiscard]] Coord getRightCoord() const {
        return {col + 1, row};
    }

    using DirectionFunction = Coord (Coord::*)() const;

    static std::array<DirectionFunction, 4> getDirectionFunctions() {
        std::array<DirectionFunction, 4> directions = {
                &Coord::getLeftCoord,
                &Coord::getDownCoord,
                &Coord::getRightCoord,
                &Coord::getUpCoord
        };
        return directions;
    }
};

#endif //AOC_2023_COORD_H
