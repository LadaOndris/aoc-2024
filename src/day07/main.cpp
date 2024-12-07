#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include "input.h"
#include "timer.h"
#include <sstream>


namespace {

    using OperandType = long;

    struct Equation {
        OperandType result{};
        std::vector<OperandType> operarands{};
    };

    std::vector<Equation> loadInput(std::vector<std::string> &lines) {
        std::vector<Equation> equations(lines.size());

        for (int i = 0; i < lines.size(); i++) {
            auto &line = lines[i];
            auto &equation = equations[i];

            std::istringstream iss(line);

            std::string equationResultString{};
            std::getline(iss, equationResultString, ':');
            equation.result = std::stol(equationResultString);

            iss.ignore(1);

            std::string operandString{};
            while (iss >> operandString) {
                OperandType operand = std::stol(operandString);
                equation.operarands.push_back(operand);
            }
        }

        return equations;
    }

    struct OperatorApplication {
        OperandType result;
    };

    using OperatorFunction = std::function<OperandType(OperandType, OperandType)>;

    bool findOperators(std::vector<OperatorApplication> &applications,
                       const Equation &equation,
                       const std::vector<OperatorFunction> &operators) {
        bool allOperandsUsedUp = applications.size() == equation.operarands.size() - 1;
        if (allOperandsUsedUp) {
            return applications.back().result == equation.result;
        }

        size_t index = applications.size();
        OperandType leftOperand = (index == 0) ? equation.operarands[index] : applications.back().result;
        OperandType rightOperand = equation.operarands[index + 1];

        for (const auto &func: operators) {
            OperandType result = func(leftOperand, rightOperand);
            if (result > equation.result) {
                continue;
            }
            applications.push_back(OperatorApplication{result});

            if (findOperators(applications, equation, operators)) {
                return true;
            }
            applications.pop_back();
        }

        return false;
    }

    OperandType findCalibrationResult(const std::vector<Equation> &equations,
                              const std::function<bool(const Equation &)> &isEquationValidFunc) {
        auto sumValidEquations = [&isEquationValidFunc](OperandType sum, const Equation &equation) {
            if (isEquationValidFunc(equation)) {
                return sum + equation.result;
            }
            return sum;
        };

        auto validEquationsSum = std::accumulate(equations.begin(), equations.end(), static_cast<OperandType>(0),
                                                 sumValidEquations);
        return validEquationsSum;
    }

}

namespace Part1 {

    bool isEquationValid(const Equation &equation) {
        const std::vector<OperatorFunction> operators = {
                [](OperandType a, OperandType b) { return a + b; },
                [](OperandType a, OperandType b) { return a * b; }
        };
        std::vector<OperatorApplication> applications{};
        return findOperators(applications, equation, operators);
    }

    void execute(std::vector<std::string> &lines) {
        auto equations = loadInput(lines);
        auto validEquationsSum = findCalibrationResult(equations, isEquationValid);
        std::cout << validEquationsSum << std::endl;
    }
}

namespace Part2 {

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
        std::vector<OperatorApplication> applications{};
        return findOperators(applications, equation, operators);
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
    Part1::execute(lines);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    Part2::execute(lines);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
