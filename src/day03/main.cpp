#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <optional>
#include <charconv>
#include <regex>
#include "input.h"

namespace {

    std::string loadInput(const std::string &filename) {
        return input::readFile<std::string>(filename, input::read);
    }

    struct MulInstruction {
        long firstNumber;
        long secondNumber;

        [[nodiscard]] long calculate() const {
            return firstNumber * secondNumber;
        }

        friend std::ostream &operator<<(std::ostream &out, const MulInstruction &instr) {
            out << "(" << instr.firstNumber << "," << instr.secondNumber << ")";
            return out;
        }
    };

    class Parser {
    public:
        std::vector<std::pair<MulInstruction, int>> extractValidInstructions(const std::string &inputString) {
            std::vector<std::pair<MulInstruction, int>> instructions;
            std::regex pattern(R"(mul\((\d+),(\d+)\))");
            std::sregex_iterator it(inputString.begin(), inputString.end(), pattern);
            std::sregex_iterator end;

            for (; it != end; ++it) {
                auto position = static_cast<int>(it->position());

                MulInstruction instruction{
                        std::stol(it->str(1)),
                        std::stol(it->str(2))
                };
                instructions.emplace_back(instruction, position);
            }
            return instructions;
        }

        std::vector<int> extractPositionsOfDoInstructions(const std::string &inputString) {
            std::vector<int> positions;
            std::regex pattern(R"(do\(\))");
            std::sregex_iterator it(inputString.begin(), inputString.end(), pattern);
            std::sregex_iterator end;

            for (; it != end; ++it) {
                auto position = static_cast<int>(it->position());
                positions.push_back(position);
            }
            return positions;
        }

        std::vector<int> extractPositionsOfDontInstructions(const std::string &inputString) {
            std::vector<int> positions;
            std::regex pattern(R"(don't\(\))");
            std::sregex_iterator it(inputString.begin(), inputString.end(), pattern);
            std::sregex_iterator end;

            for (; it != end; ++it) {
                auto position = static_cast<int>(it->position());
                positions.push_back(position);
            }
            return positions;
        }
    };
}

namespace part1 {


    long sumInstructionResults(const std::vector<std::pair<MulInstruction, int>> &instructions) {
        return std::accumulate(
                instructions.begin(),
                instructions.end(),
                0l, // Initial value of the sum
                [](long sum, const std::pair<MulInstruction, int> &instruction) {
                    return sum + instruction.first.calculate();
                });
    }

    template<typename Input>
    void execute(Input &input) {
        Parser parser{};
        auto instructions = parser.extractValidInstructions(input);

        long total = sumInstructionResults(instructions);

        std::cout << total << std::endl;
    }
}

namespace part2 {

    std::vector<MulInstruction> extractEnabledInstructions(
            const std::vector<std::pair<MulInstruction, int>> &instructions,
            const std::vector<int> &doPositions,
            const std::vector<int> &dontPositions) {

        std::vector<MulInstruction> enabledInstructions;

        bool enabled = true;
        auto doPositionsIter = doPositions.begin();
        auto dontPositionsIter = dontPositions.begin();

        int lastProcessedPosition = -1;

        for (const auto &[instruction, position]: instructions) {
            while (*doPositionsIter < position && doPositionsIter != doPositions.end()) {
                if (*doPositionsIter > lastProcessedPosition) {
                    enabled = true;
                    lastProcessedPosition = *doPositionsIter;
                }
                doPositionsIter++;
            }
            while (*dontPositionsIter < position && dontPositionsIter != dontPositions.end()) {
                if (*dontPositionsIter > lastProcessedPosition) {
                    enabled = false;
                    lastProcessedPosition = *dontPositionsIter;
                }
                dontPositionsIter++;
            }
            if (enabled) {
                enabledInstructions.push_back(instruction);
            }
        }
        return enabledInstructions;
    }

    long sumInstructionResults(const std::vector<MulInstruction> &instructions) {
        return std::accumulate(
                instructions.begin(),
                instructions.end(),
                0l, // Initial value of the sum
                [](long sum, const MulInstruction &instruction) {
                    return sum + instruction.calculate();
                });
    }

    template<typename Input>
    void execute(Input &input) {
        Parser parser{};
        auto instructions = parser.extractValidInstructions(input);
        auto doInstructionPositions = parser.extractPositionsOfDoInstructions(input);
        auto dontInstructionPositions = parser.extractPositionsOfDontInstructions(input);

        auto enabledInstructions = extractEnabledInstructions(instructions, doInstructionPositions,
                                                              dontInstructionPositions);

        long total = sumInstructionResults(enabledInstructions);

        std::cout << total << std::endl;
    }
}

int main() {
    auto input = loadInput("day03.txt");

    part1::execute(input);
    part2::execute(input);

    return 0;
}
