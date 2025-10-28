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

    struct Space {
        bool isFile;
        int allocatedSpace;
        int occupiedSpace;
    };

    struct DiskMap {

    };

    DiskMap loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        

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
