#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <optional>
#include <unordered_set>
#include <list>
#include "input.h"
#include "array2d.h"
#include "Coord.h"
#include "Direction.h"
#include "timer.h"


namespace {

    enum FieldType {
        Empty,
        Obstruction,
        Guard
    };

    std::unordered_map<char, FieldType> charToFieldTypeMap{
            {'.', Empty},
            {'#', Obstruction},
            {'^', Guard}};

    std::unordered_map<FieldType, char> fieldTypeToCharMap{
            {Empty,       '.'},
            {Obstruction, '#'},
            {Guard,       '^'}};


    std::ostream &operator<<(std::ostream &out, const FieldType &data) {
        out << fieldTypeToCharMap.at(data);
        return out;
    }

    // TODO: move to print.h
    template<typename T>
    std::ostream &operator<<(std::ostream &out, const Array2D<T> &data) {
        out << '[' << std::endl;
        for (size_t row = 0; row < data.rows(); row++) {
            out << '[';
            for (size_t col = 0; col < data.cols() - 1; col++) {
                out << data(row, col) << ',';
            }
            out << data(row, data.cols() - 1);
            out << ']' << std::endl;
        }
        out << ']';
        return out;
    }

    using Map = Array2D<FieldType>;

    FieldType transformCharToFieldType(char character) {
        return charToFieldTypeMap.at(character);
    }

    Map loadInput(const std::vector<std::string> &lines) {
        std::function<FieldType(char)> transformFunction = transformCharToFieldType;
        return input::load2D(lines, transformFunction);
    }

    Coord findGuardPosition(const Map &map) {
        auto it = std::find(map.cbegin(), map.cend(), FieldType::Guard);
        if (it != map.cend()) {
            size_t index = std::distance(map.cbegin(), it);

            int row = static_cast<int>(index) / map.cols();
            int col = static_cast<int>(index) % map.cols();

            return Coord{.col=col, .row=row};
        }
        throw std::runtime_error("No guard was found.");
    }
}

namespace Part1 {

    struct CoordDirectionHash {
        std::size_t operator( )(const std::pair<Coord, Direction> &p) const {
            CoordHash coordHash;
            auto h1 = coordHash(p.first);
            auto h2 = std::hash<Direction>{}(p.second);
            return h1 * 31 + h2;
        }
    };

    class Guard {
    public:
        explicit Guard(Coord position)
                : position(position),
                  direction(Direction::Up) {}


        int walk(const Map &map) {
            while (map.isInBounds(position.row, position.col)) {
                determineNextDirection(map);
                move();

                if (map.isInBounds(position.row, position.col)) {
                    markPosition();
                }
            }

            return static_cast<int>(visitedCoords.size());
        }

        std::unordered_set<Coord, CoordHash> getVisitedCoords() const {
            return visitedCoords;
        }

        bool walkDetectLoop(const Map &map) {
            Coord startPosition = position;
            std::unordered_set<std::pair<Coord, Direction>, CoordDirectionHash> visitedBefore;
            visitedBefore.insert({startPosition, direction});

            while (map.isInBounds(position.row, position.col)) {
                determineNextDirection(map);
                move();

                if (map.isInBounds(position.row, position.col)) {
                    if (visitedBefore.count({position, direction}) > 0) {
                        return true;
                    }

                    visitedBefore.insert({position, direction});
                }
            }

            return false;
        }

    private:

        /**
         * Looks if there is an obstacle directly in front of it and rotates if there is.
         * It keeps rotating until there is an empty tile in front of it.
         */
        void determineNextDirection(const Map &map) {
            while (isObstructionInFront(map)) {
                changeDirection();
            }
        }

        bool isObstructionInFront(const Map &map) {
            auto nextCoord = getNextCoordInCurrentDirection();
            if (map.isInBounds(nextCoord.row, nextCoord.col)) {
                return map(nextCoord.row, nextCoord.col) == FieldType::Obstruction;
            }
            return false;
        }

        void changeDirection() {
            switch (direction) {
                case Direction::Up:
                    direction = Direction::Right;
                    break;
                case Direction::Down:
                    direction = Direction::Left;
                    break;
                case Direction::Left:
                    direction = Direction::Up;
                    break;
                case Direction::Right:
                    direction = Direction::Down;
                    break;
                default:
                    throw std::invalid_argument("Invalid direction");
            }
        }

        Coord getNextCoordInCurrentDirection() {
            switch (direction) {
                case Direction::Up:
                    return position.getUpCoord();
                case Direction::Down:
                    return position.getDownCoord();
                case Direction::Left:
                    return position.getLeftCoord();
                case Direction::Right:
                    return position.getRightCoord();
                default:
                    throw std::invalid_argument("Invalid direction");
            }

        }

        /**
         * Moves guard in its currently set direction. Assumes no obstacle in front of it.
         */
        void move() {
            switch (direction) {
                case Direction::Up:
                    position = position.getUpCoord();
                    break;
                case Direction::Down:
                    position = position.getDownCoord();
                    break;
                case Direction::Left:
                    position = position.getLeftCoord();
                    break;
                case Direction::Right:
                    position = position.getRightCoord();
                    break;
                default:
                    throw std::invalid_argument("Invalid direction");
            }
        }

        void markPosition() {
            visitedCoords.insert(position);
        }

        Coord position;
        Direction direction;
        std::unordered_set<Coord, CoordHash> visitedCoords;
    };

    void execute(std::vector<std::string> &lines) {
        auto map = loadInput(lines);

        std::cout << map << std::endl;

        auto guardPosition = findGuardPosition(map);
        Guard guard(guardPosition);
        int numVisitedTiles = guard.walk(map);

        std::cout << numVisitedTiles << std::endl;
    }
}

namespace Part2 {

    using Guard = Part1::Guard;

    bool placeObstacleAndDetectLoop(const Coord &startingPosition, const Coord &coord, Map &map) {
        // Cannot place an obstruction where the guard is standing.
        if (startingPosition == coord) {
            return false;
        }

        map(coord.row, coord.col) = FieldType::Obstruction;

        Guard guard(startingPosition);
        bool isLoop = guard.walkDetectLoop(map);

        map(coord.row, coord.col) = FieldType::Empty;
        return isLoop;
    }

    int
    createLoops(const std::unordered_set<Coord, CoordHash> &visitedCoords, const Coord &startingPosition, Map &map) {
        long numCreatedLoops = std::count_if(visitedCoords.begin(), visitedCoords.end(),
                                             [&startingPosition, &map](const Coord &coord) {
                                                 return placeObstacleAndDetectLoop(startingPosition, coord, map);
                                             });
        return static_cast<int>(numCreatedLoops);
    }
;
    void execute(std::vector<std::string> &lines) {
        auto map = loadInput(lines);

        auto guardPosition = findGuardPosition(map);
        Guard guard(guardPosition);
        guard.walk(map);
        auto visitedCoords = guard.getVisitedCoords();

        int numLoops = createLoops(visitedCoords, guardPosition, map);

        std::cout << numLoops << std::endl;
    }
}

int main() {
    auto lines = input::readFile<std::vector<std::string>>("day06.txt", input::readLines);

    Timer timer;
    Part1::execute(lines);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    Part2::execute(lines);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
