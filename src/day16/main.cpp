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


namespace {

    enum class CellType {
        Empty,
        Wall,
        Start,
        End
    };

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
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        Input input{};

        input.map = input::load2D<CellType>(lines, [](char c) {
            switch (c) {
                case '.': return CellType::Empty;
                case '#': return CellType::Wall;
                case 'S': return CellType::Start;
                case 'E': return CellType::End;
                default: throw std::runtime_error("Unknown cell type: " + std::string(1, c));
            }
        });

        return input;
    }

}

namespace part1 {

    template <typename Input>
    void execute(const Input &input) {
        std::cout << input.map << std::endl;
    }
}

namespace part2 {

    template <typename Input>
    void execute(const Input &input) {

    }
}

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
