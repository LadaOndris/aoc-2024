#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
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

enum class CellType { Empty, Wall, Start, End, Path };

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
        case CellType::Path:
            out << 'o';
            break;
    }
    return out;
}

struct Input {
    Array2D<CellType> map;
};

Input loadInput(const std::string &filename) {
    auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
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
                throw std::runtime_error("Unknown cell type: " + std::string(1, c));
        }
    });

    return input;
}

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
        return std::hash<int>()(position.coord.row) ^ std::hash<int>()(position.coord.col) ^
               std::hash<int>()(static_cast<int>(position.direction));
    }
};

struct PositionEqual {
    bool operator()(const Position &a, const Position &b) const {
        return a.coord == b.coord && a.direction == b.direction;
    }
};

Coord getPos(const Array2D<CellType> &map, const CellType target) {
    auto it = std::find_if(map.cbegin(), map.cend(), [target](CellType cell) { return cell == target; });
    if (it != map.cend()) {
        size_t col = std::distance(map.cbegin(), it) % map.cols();
        size_t row = std::distance(map.cbegin(), it) / map.cols();
        return Coord{.col = static_cast<int>(col), .row = static_cast<int>(row)};
    }
    throw std::runtime_error("Target position not found");
}

struct SearchResult {
    int totalCost = -1;
    std::unordered_set<Position, PositionHash, PositionEqual> closedSet{};
};

SearchResult findPath(const Input &input, const Coord &start, const Coord &end) {
    SearchResult result{};

    // Keep a priority queue of explored positions according to their current
    // price. They are to be removed once processed. An explored position is
    // identified by the coord and the direction it was reached from.
    std::priority_queue<Position, std::vector<Position>, CostComparer> openSet;
    openSet.push({start, Direction::Right, 0});

    // Keep a set of visited positions - with their best prices and the
    // direction from which they were reached.
    auto &closedSet = result.closedSet;

    while (!openSet.empty()) {
        Position current = openSet.top();
        openSet.pop();

        if (current.coord == end) {
            std::cout << "Found path to end with cost: " << current.cost << std::endl;
            closedSet.insert(current);
            result.totalCost = current.cost;
            break;
        }

        // Skip if already processed
        if (closedSet.count(current)) {
            continue;
        }

        closedSet.insert(current);

        for (const auto &direction : getAllDirections()) {
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
    return result;
}

}  // namespace

namespace part1 {

template <typename Input>
void execute(const Input &input) {
    auto start = getPos(input.map, CellType::Start);
    auto end = getPos(input.map, CellType::End);
    std::cout << "Start: " << start << ", End: " << end << std::endl;

    auto searchResult = findPath(input, start, end);
    std::cout << "Total cost of optimal path: " << searchResult.totalCost << std::endl;
}

}  // namespace part1

namespace part2 {

void traceback(const std::unordered_set<Position, PositionHash, PositionEqual> &closedSet, const Coord &start,
               const Coord &current, const Direction inDirection, std::unordered_set<Coord> &tiles) {
    // Search for the neighbors with the least price.
    std::unordered_set<Position, PositionHash, PositionEqual> neighbors{};
    int bestPrice = std::numeric_limits<int>::max();

    if (current == start) {
        tiles.insert(start);
        return;
    }

    // The neighbor could have come from any direction.
    for (const auto &direction : getAllDirections()) {
        Position pos{.coord = current, .direction = direction, .cost = 0};

        auto neighborIt = closedSet.find(pos);
        if (neighborIt != closedSet.end()) {
            auto neighborCopy = *neighborIt;
            // Take into account that the direction may be changing.
            // This hack is needed to trace back according to the price.
            neighborCopy.cost += (getOpposite(inDirection) == neighborIt->direction) ? 0 : 1000;

            neighbors.insert(neighborCopy);
            bestPrice = std::min(bestPrice, neighborCopy.cost);
        }
    }

    // Trace back to the neighbors with the best price
    for (const auto &neighbor : neighbors) {
        if (neighbor.cost == bestPrice) {
            tiles.insert(neighbor.coord);

            auto oppositeDirection = getOpposite(neighbor.direction);
            auto traceBackTile = neighbor.coord + oppositeDirection;

            traceback(closedSet, start, traceBackTile, oppositeDirection, tiles);
        }
    }
}

template <typename Input>
void execute(const Input &input, bool verbose = false) {
    auto start = getPos(input.map, CellType::Start);
    auto end = getPos(input.map, CellType::End);
    std::cout << "Start: " << start << ", End: " << end << std::endl;

    auto searchResult = findPath(input, start, end);

    std::unordered_set<Coord> tilesOnAnyPath{};
    traceback(searchResult.closedSet, start, end, Direction::Up, tilesOnAnyPath);
    std::cout << "Tiles on any optimal path: " << tilesOnAnyPath.size() << std::endl;

    if (verbose) {
        Array2D<CellType> outputMap = input.map;
        for (const auto &tile : tilesOnAnyPath) {
            outputMap(tile.row, tile.col) = CellType::Path;
        }
        std::cout << outputMap << std::endl;
    }
}

}  // namespace part2

int main() {
    auto input = loadInput("day16.txt");
    bool verbose = false;

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input, verbose);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
