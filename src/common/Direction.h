
#ifndef AOC_2023_DIRECTION_H
#define AOC_2023_DIRECTION_H

#include <stdexcept>
#include <ostream>

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

inline Direction getOpposite(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
        case Direction::Left:
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
        default:
            throw std::runtime_error("Unknown direction");
    }
}

inline std::ostream &operator<<(std::ostream &out, const Direction &direction) {
    switch (direction) {
        case Direction::Up:
            out << "Up";
            break;
        case Direction::Down:
            out << "Down";
            break;
        case Direction::Left:
            out << "Left";
            break;
        case Direction::Right:
            out << "Right";
            break;
    }
    return out;
}

#endif
