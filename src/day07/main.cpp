#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include "input.h"
#include "timer.h"
#include <sstream>
#include <execution>


namespace {

    using OperandType = uint64_t;

    struct Equation {
        OperandType result{};
        std::vector<OperandType> operands{};
    };

    std::vector<Equation> loadInput(std::vector<std::string> &lines) {
        std::vector<Equation> equations(lines.size());

        for (int i = 0; i < lines.size(); i++) {
            auto &line = lines[i];
            auto &equation = equations[i];

            auto parts = input::split(line, ':');
            equation.result = std::stol(parts[0]);
            equation.operands = input::parseVector<OperandType>(parts[1], ' ');
        }

        return equations;
    }

    using OperatorFunction = std::function<OperandType(OperandType, OperandType)>;

    bool findOperators(const Equation &equation,
                       OperandType result,
                       int index,
                       const std::vector<OperatorFunction> &operators) {

        bool allOperandsUsedUp = static_cast<size_t>(index) == equation.operands.size() - 1;
        if (allOperandsUsedUp) {
            return result == equation.result;
        }

        for (const auto &func: operators) {
            OperandType subresult = func(result, equation.operands[index + 1]);
            if (result > equation.result) {
                continue;
            }

            if (findOperators(equation, subresult, index + 1, operators)) {
                return true;
            }
        }
        return false;
    }

    OperandType findCalibrationResult(const std::vector<Equation> &equations,
                                      const std::function<bool(const Equation &)> &isEquationValidFunc) {

        // Lambda to compute the contribution of a single equation if it is valid
        auto calculateContribution = [&isEquationValidFunc](const Equation &equation) -> OperandType {
            return isEquationValidFunc(equation) ? equation.result : static_cast<OperandType>(0);
        };

        OperandType validEquationsSum = std::transform_reduce(
                std::execution::par, // Parallel execution policy
                equations.begin(), equations.end(),
                static_cast<OperandType>(0), // Initial value for the sum
                std::plus<OperandType>(),    // Binary operation to combine results
                calculateContribution        // Transformation function
        );

        return validEquationsSum;
    }
}

namespace part1 {

    bool isEquationValid(const Equation &equation) {
        const std::vector<OperatorFunction> operators = {
                [](OperandType a, OperandType b) { return a + b; },
                [](OperandType a, OperandType b) { return a * b; }
        };
        return findOperators(equation, equation.operands[0], 0, operators);
    }

    void execute(std::vector<std::string> &lines) {
        auto equations = loadInput(lines);
        auto validEquationsSum = findCalibrationResult(equations, isEquationValid);
        std::cout << validEquationsSum << std::endl;
    }
}

namespace part2 {

    bool isEquationValid(const Equation &equation) {
        const std::vector<OperatorFunction> operators = {
                [](OperandType a, OperandType b) { return a + b; },
                [](OperandType a, OperandType b) { return a * b; },
                [](OperandType a, OperandType b) {
                    OperandType multiplier = 1;
                    while (multiplier <= b) {
                        multiplier *= 10;
                    }
                    return a * multiplier + b;
                }
        };
        return findOperators(equation, equation.operands[0], 0, operators);
    }

    void execute(std::vector<std::string> &lines) {
        auto equations = loadInput(lines);
        auto validEquationsSum = findCalibrationResult(equations, isEquationValid);
        std::cout << validEquationsSum << std::endl;
    }
}

int main() {
    auto lines = input::readFile<std::vector<std::string>>("day07.txt", input::readLines);

    Timer timer;
    part1::execute(lines);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(lines);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
