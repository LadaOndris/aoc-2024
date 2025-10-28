#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include "Direction.h"
#include "array2d.h"
#include "input.h"
#include "print.h"
#include "timer.h"

namespace {

enum class CellType { Empty, Wall, Start, End };

std::ostream &operator<<(std::ostream &out, const CellType &cellType) {
    switch (cellType) {
        case CellType::Empty:
            out << '.';
            break;
        case CellType::Wall:
            out << '#';
            break;
        case CellType::Start:
            out << 'S';
            break;
        case CellType::End:
            out << 'E';
            break;
    }
    return out;
}

struct Input {
    Array2D<CellType> map;
};

Input loadInput(const std::string &filename) {
    auto lines =
        input::readFile<std::vector<std::string>>(filename, input::readLines);
    Input input{};

    input.map = input::load2D<CellType>(lines, [](char c) {
        switch (c) {
            case '.':
                return CellType::Empty;
            case '#':
                return CellType::Wall;
            case 'S':
                return CellType::Start;
            case 'E':
                return CellType::End;
            default:
                throw std::runtime_error("Unknown cell type: " +
                                         std::string(1, c));
        }
    });

    return input;
}

}  // namespace

namespace part1 {

struct Position {
    Coord coord;
    Direction direction;
    int cost;
};

struct CostComparer {
    bool operator()(const Position &a, const Position &b) const {
        return a.cost > b.cost;
    }
};

struct PositionHash {
    std::size_t operator()(const Position &position) const {
        return std::hash<int>()(position.coord.row) ^
               std::hash<int>()(position.coord.col) ^
               std::hash<int>()(static_cast<int>(position.direction));
    }
};

struct PositionEqual {
    bool operator()(const Position &a, const Position &b) const {
        return a.coord == b.coord && a.direction == b.direction;
    }
};

Coord getPos(const Array2D<CellType> &map, const CellType target) {
    auto it = std::find_if(map.cbegin(), map.cend(),
                           [target](CellType cell) { return cell == target; });
    if (it != map.cend()) {
        size_t col = std::distance(map.cbegin(), it) % map.cols();
        size_t row = std::distance(map.cbegin(), it) / map.cols();
        return Coord{.col = static_cast<int>(col),
                     .row = static_cast<int>(row)};
    }
    throw std::runtime_error("Target position not found");
}

template <typename Input>
void execute(const Input &input) {
    std::cout << input.map << std::endl;

    auto start = getPos(input.map, CellType::Start);
    auto end = getPos(input.map, CellType::End);
    std::cout << "Start: " << start << ", End: " << end << std::endl;

    // Keep a priority queue of explored positions according to their current
    // price. They are to be removed once processed. An explored position is
    // identified by the coord and the direction it was reached from.
    std::priority_queue<Position, std::vector<Position>, CostComparer> openSet;
    openSet.push({start, Direction::Right, 0});

    // Keep a set of visited positions - with their best prices and the
    // direction from which they were reached.
    std::unordered_set<Position, PositionHash, PositionEqual> closedSet{};

    while (!openSet.empty()) {
        Position current = openSet.top();
        openSet.pop();

        if (current.coord == end) {
            std::cout << "Found path to end with cost: " << current.cost
                      << std::endl;
            return;
        }

        // Skip if already processed
        if (closedSet.count(current)) {
            continue;
        }

        closedSet.insert(current);

        for (const auto &direction : {Direction::Up, Direction::Down,
                                      Direction::Left, Direction::Right}) {
            if (direction == getOpposite(current.direction)) {
                continue;
            }
            Coord neighbor = current.coord + direction;

            if (!input.map.isInBounds(neighbor.row, neighbor.col) ||
                input.map(neighbor.row, neighbor.col) == CellType::Wall) {
                continue;
            }

            int additionalCost = (direction == current.direction) ? 1 : 1001;
            int neighborCost = current.cost + additionalCost;
            openSet.push({neighbor, direction, neighborCost});
        }
    }
}
}  // namespace part1

namespace part2 {

template <typename Input>
void execute(const Input &input) {}

}  // namespace part2

int main() {
    auto input = loadInput("day16.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
