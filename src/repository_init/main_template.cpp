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


namespace {

    struct Input {

    };

    Input loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        Input input{};

        return input;
    }

}

namespace part1 {

    template <typename Input>
    void execute(Input &input) {

    }
}

namespace part2 {

    template <typename Input>
    void execute(Input &input) {

    }
}

int main() {
    auto input = loadInput("day01.txt");

    part1::execute(input);
    part2::execute(input);

    return 0;
}
