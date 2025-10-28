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


namespace {

    struct Configuration {
        Coord buttonA;
        Coord buttonB;
        Coord prizeLocation;
    };

    Configuration parseConfiguration(const std::string &string) {
        auto matches = input::parseVector(string, R"(.*\+(\d+),\s*Y\+(\d+)\s*.*\+(\d+),\s*Y\+(\d+)\s*.*X=(\d+),\s*Y=(\d+))");
        auto numbers = input::convertStringsToNumbers<int>(matches);
        Coord buttonA{.col=numbers[0], .row=numbers[1]};
        Coord buttonB{.col=numbers[2], .row=numbers[3]};
        Coord prizeLocation{.col=numbers[4], .row=numbers[5]};
        return {buttonA, buttonB, prizeLocation};
    }

    std::vector<Configuration> loadInput(const std::string &filename) {
        auto content = input::readFile<std::string>(filename, input::read);
        auto parts = input::split(content, "\n\n", input::Blanks::Remove);

        std::vector<Configuration> configurations{};
        configurations.reserve(parts.size());

        std::for_each(parts.cbegin(), parts.cend(), [&configurations](const std::string &part) {
            configurations.push_back(parseConfiguration(part));
        });

        return configurations;
    }

}

namespace part1 {

    bool hasSolution(const Configuration &config) {
        // Use determinant to calculate det(A) = a1 * b2 − a2 * b1, det != 0 => has solution
        auto a1 = config.buttonA.col;
        auto a2 = config.buttonA.row;
        auto b1 = config.buttonB.col;
        auto b2 = config.buttonB.row;
        auto det = a1 * b2 - a2 * b1;
        return det != 0;
    }

    template<typename Input>
    void execute(const Input &input) {
        const std::vector<Configuration> &configurations = input;
        for (const auto &config : configurations) {
            if (hasSolution(config)) {
                std::cout << "has solution:" << config.prizeLocation << " " << config.buttonA << " " << config.buttonB << std::endl;
            }
        }
    }
}

namespace part2 {

    template<typename Input>
    void execute(const Input &input) {

    }
}

int main() {
    auto input = loadInput("day13.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
