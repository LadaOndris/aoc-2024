#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <optional>
#include <charconv>
#include "input.h"
#include "print.h"


namespace {

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

    enum class ParserState {
        None,
        ProcessedM,
        ProcessedU,
        ProcessedL,
        ProcessedLeftParenthesis,
        ProcessedComma,
        ProcessedRightParenthesis
    };

    class Parser {
    public:
        explicit Parser() : state(ParserState::None) {}

        std::vector<MulInstruction> extractValidInstructions(const std::string &inputString) {
            std::vector<MulInstruction> instructions;
            for (const auto &character: inputString) {
                processCharacter(character);

                if (state == ParserState::ProcessedRightParenthesis) {
                    auto mulInstruction = createMulInstruction();
                    instructions.push_back(mulInstruction);
                    state = ParserState::None;
                }
            }
            return instructions;
        }


    private:
        ParserState state;
        std::vector<char> firstNumber;
        std::vector<char> secondNumber;

        void processCharacter(char character) {
            using
            enum ParserState;

            switch (state) {
                case None:
                    if (character == 'm') {
                        state = ProcessedM;
                        firstNumber.clear();
                        secondNumber.clear();
                    }
                    break;
                case ProcessedM:
                    if (character == 'u') {
                        state = ProcessedU;
                    } else {
                        state = None;
                    }
                    break;
                case ProcessedU:
                    if (character == 'l') {
                        state = ProcessedL;
                    } else {
                        state = None;
                    }
                    break;
                case ProcessedL:
                    if (character == '(') {
                        state = ProcessedLeftParenthesis;
                    } else {
                        state = None;
                    }
                    break;
                case ProcessedLeftParenthesis:
                    if (character >= '0' && character <= '9') {
                        firstNumber.push_back(character);
                    } else if (character == ',') {
                        if (firstNumber.empty()) {
                            state = None;
                        } else {
                            state = ProcessedComma;
                        }
                    }
                    else {
                        state = None;
                    }
                    break;
                case ProcessedComma:
                    if (character >= '0' && character <= '9') {
                        secondNumber.push_back(character);
                    } else if (character == ')') {
                        if (secondNumber.empty()) {
                            state = None;
                        } else {
                            state = ProcessedRightParenthesis;
                        }
                    }
                    else {
                        state = None;
                    }
                    break;
                case ProcessedRightParenthesis:
                    std::cout << "Unexpected processing of ProcessedRightParenthesis state." << std::endl;
                    break;
            }
        }

        MulInstruction createMulInstruction() {
            return {.firstNumber=convertToInt(firstNumber),
                    .secondNumber=convertToInt(secondNumber)};
        }

        [[nodiscard]] int convertToInt(const std::vector<char> &characters) {
            if (characters.empty()) {
                throw std::invalid_argument("Input vector is empty.");
            }

            int result = 0;
            auto [ptr, ec] = std::from_chars(characters.data(), characters.data() + characters.size(), result);

            if (ec != std::errc{}) {
                throw std::invalid_argument("Invalid input for conversion.");
            }

            return result;
        }
    };
}

namespace Part1 {

    void execute(std::string &content) {
        Parser parser{};
        auto instructions = parser.extractValidInstructions(content);
        std::cout << instructions << std::endl;
        long total = std::accumulate(
                instructions.begin(),
                instructions.end(),
                0l, // Initial value of the sum
                [](long sum, const MulInstruction& instruction) {
                    return sum + instruction.calculate();
                });
        std::cout << total << std::endl;
    }
}

namespace Part2 {

    void execute(std::string &content) {

    }
}

int main() {
    auto content = input::readFile<std::string>("day03.txt", input::read);

    Part1::execute(content);
    Part2::execute(content);

    return 0;
}
