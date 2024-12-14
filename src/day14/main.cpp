#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <optional>
#include "input.h"
#include "print.h"
#include "array2d.h"
#include "timer.h"
#include "Coord.h"


namespace {

    struct Robot {
        Coord position;
        Coord velocity;
    };

    Robot parseRobot(const std::string &line) {
        auto parts = input::parseVector(line, R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+))");
        auto numbers = input::convertStringsToNumbers<int>(parts);
        Coord position{.col=numbers[0], .row=numbers[1]};
        Coord velocity{.col=numbers[2], .row=numbers[3]};
        return Robot{position, velocity};
    }

    std::vector<Robot> loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        std::vector<Robot> robots{};

        for (const auto &line: lines) {
            robots.push_back(parseRobot(line));
        }
        return robots;
    }

}

namespace part1 {

    void setNewRobotPositions(std::vector<Robot> &robots, int width, int height, int seconds) {
        for (auto &robot: robots) {
            auto &position = robot.position;
            auto &velocity = robot.velocity;

            auto newPosX = (position.col + (velocity.col * seconds)) % width;
            auto newPosY = (position.row + (velocity.row * seconds)) % height;

            // Handle negative numbers
            newPosX = (newPosX + width) % width;
            newPosY = (newPosY + height) % height;

            position.col = newPosX;
            position.row = newPosY;
        }
    }

    int determineSafetyFactor(const std::vector<Robot> &robots, int width, int height) {
        int xThreshold = (width - 1) / 2;
        int yThreshold = (height - 1) / 2;

        int topLeft{};
        int topRight{};
        int bottomLeft{};
        int bottomRight{};

        for (const auto &robot: robots) {
            const auto &pos = robot.position;

            if (pos.col < xThreshold && pos.row < yThreshold) {
                topLeft++;
            } else if (pos.col < xThreshold && pos.row > yThreshold) {
                bottomLeft++;
            } else if (pos.col > xThreshold && pos.row < yThreshold) {
                topRight++;
            } else if (pos.col > xThreshold && pos.row > yThreshold) {
                bottomRight++;
            }
        }
        return topLeft * topRight * bottomLeft * bottomRight;
    }

    template<typename Input>
    void execute(const Input &input) {
        int durationInSeconds = 100;
        int width = 101;
        int height = 103;

        std::vector<Robot> robots = input;
        setNewRobotPositions(robots, width, height, durationInSeconds);
        auto safetyFactor = determineSafetyFactor(robots, width, height);

        std::cout << safetyFactor << std::endl;
    }
}

namespace part2 {

    template<typename Iterator>
    int getMaxRobotsInSequence(Iterator begin, Iterator end) {
        int maxRobotsInLine = 0;
        int currentRobotsInLine = 0;

        for (auto it = begin; it != end; ++it) {
            if (*it == 1) {
                currentRobotsInLine++;
            } else {
                maxRobotsInLine = std::max(maxRobotsInLine, currentRobotsInLine);
                currentRobotsInLine = 0;
            }
        }
        maxRobotsInLine = std::max(maxRobotsInLine, currentRobotsInLine);
        return maxRobotsInLine;
    }

    bool isTreeCandidate(Array2D<int> &map, int width, int height, int minRobotsInSequence) {
        int maxRobotsInLine = 0;
        for (int row = 0; row < map.rows(); row++) {
            auto robotsInLine = getMaxRobotsInSequence(map.rowBegin(row), map.rowEnd(row));
            maxRobotsInLine = std::max(maxRobotsInLine, robotsInLine);
        }

        return maxRobotsInLine >= minRobotsInSequence;
    }

    template<typename Input>
    void execute(const Input &input) {
        int width = 101;
        int height = 103;
        int minRobotsInSequence = 10;

        std::vector<Robot> robots{};
        Array2D<int> map(height, width);
        int durationInSeconds = 0;
        do {
            robots = input;
            durationInSeconds++;
            part1::setNewRobotPositions(robots, width, height, durationInSeconds);

            std::fill(map.begin(), map.end(), 0);
            for (const auto &robot: robots) {
                map(robot.position.row, robot.position.col) = 1;
            }
        } while (!isTreeCandidate(map, width, height, minRobotsInSequence));

        std::cout << "Tree candidate:" << durationInSeconds << std::endl;
        // std::cout << map << std::endl;
    }
}

int main() {
    auto input = loadInput("day14.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
