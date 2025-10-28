
#pragma once

#include <array>
#include <ostream>
#include <stdexcept>

enum class Direction { Up, Down, Left, Right };

static consteval std::array<Direction, 4> getAllDirections() {
    return {{Direction::Up, Direction::Down, Direction::Left, Direction::Right}};
}

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
