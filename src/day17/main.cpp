#include "array2d.h"
#include "input.h"
#include "print.h"
#include "timer.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

    enum class Register { A = 0, B = 1, C = 2 };

    struct InitialState {
        std::array<int64_t, 3> registers;
    };

    enum class OpCode { Adv = 0, Bxl = 1, Bst = 2, Jnz = 3, Bxc = 4, Out = 5, Bdv = 6, Cdv = 7 };

    struct Instruction {
        OpCode opcode;
        int64_t operand;
    };

    struct Program {
        std::vector<Instruction> instructions;
    };

    struct Input {
        InitialState initialState;
        Program program;
    };

    bool isRegisterLine(const std::string &line) {
        return line.starts_with("Register");
    }

    bool isProgramLine(const std::string &line) {
        return line.starts_with("Program");
    }

    OpCode parseOpCode(int64_t code) {
        switch (code) {
        case 0:
            return OpCode::Adv;
        case 1:
            return OpCode::Bxl;
        case 2:
            return OpCode::Bst;
        case 3:
            return OpCode::Jnz;
        case 4:
            return OpCode::Bxc;
        case 5:
            return OpCode::Out;
        case 6:
            return OpCode::Bdv;
        case 7:
            return OpCode::Cdv;
        default:
            throw std::runtime_error("Unknown opcode: " + std::to_string(code));
        }
    }

    Input loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        Input input{};
        int registerIndex = 0;

        for (const auto &line : lines) {
            if (isRegisterLine(line)) {
                auto parts = input::split(line, ':', input::Blanks::Remove);
                input.initialState.registers[registerIndex++] = std::stoi(parts[1]);
            } else if (isProgramLine(line)) {
                auto parts = input::split(line, ':', input::Blanks::Remove);
                if (parts.size() != 2) {
                    throw std::runtime_error("Invalid program line: " + line);
                }
                auto args = input::parseVector<int64_t>(parts[1], ',');
                for (size_t i = 0; i < args.size(); i += 2) {
                    Instruction instr{};
                    instr.opcode = parseOpCode(args[i]);
                    instr.operand = args[i + 1];
                    input.program.instructions.push_back(instr);
                }
            }
        }
        return input;
    }

    std::ostream &operator<<(std::ostream &os, const OpCode &opcode) {
        switch (opcode) {
        case OpCode::Adv:
            os << "Adv";
            break;
        case OpCode::Bxl:
            os << "Bxl";
            break;
        case OpCode::Bst:
            os << "Bst";
            break;
        case OpCode::Jnz:
            os << "Jnz";
            break;
        case OpCode::Bxc:
            os << "Bxc";
            break;
        case OpCode::Out:
            os << "Out";
            break;
        case OpCode::Bdv:
            os << "Bdv";
            break;
        case OpCode::Cdv:
            os << "Cdv";
            break;
        }
        return os;
    }

    template <size_t N>
    std::ostream &operator<<(std::ostream &os, const std::array<int64_t, N> &array) {
        for (const auto &reg : array) {
            os << reg << " ";
        }
        os << "\n";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Input &input) {
        os << "Initial State:\n";
        os << "Registers: " << input.initialState.registers << "\n";
        os << "Program:\n";
        for (const auto &instr : input.program.instructions) {
            os << "Opcode: " << instr.opcode << ", Operand: " << instr.operand << "\n";
        }
        return os;
    }

    class IWriter {
      public:
        virtual void write(char value) = 0;
    };

    class Processor {
      public:
        Processor(std::shared_ptr<IWriter> writer) : _writer(writer) {
        }

        void setState(const InitialState &state) {
            _registers = state.registers;
        }

        void runProgram(const Program &program) {
            _instructionPointer = 0;
            _previousOut = false;
            _isInterrupted = false;

            while (!_isInterrupted && _instructionPointer < program.instructions.size()) {
                const auto &instr = program.instructions[_instructionPointer];
                executeInstruction(instr);
            }
        }

        void setInterrupt(bool value) {
            _isInterrupted = value;
        }

        bool isInterrupted() const {
            return _isInterrupted;
        }

        void printState() const {
            std::cout << "\nRegisters: " << _registers << "\n";
        }

      private:
        int64_t &getRegister(Register reg) {
            return _registers[static_cast<int64_t>(reg)];
        }

        int64_t getRegister(Register reg) const {
            return _registers[static_cast<int64_t>(reg)];
        }

        int64_t getComboOperandValue(int64_t operand) const {
            static constexpr int64_t base = 4;
            if (operand >= base && operand <= base + 2) {
                return getRegister(static_cast<Register>(operand - base));
            } else {
                return operand;
            }
        }

        int64_t calcDv(int64_t operand) {
            auto combo = getComboOperandValue(operand);
            auto numerator = getRegister(Register::A);
            return numerator / std::pow(2, combo);
        }

        void executeInstruction(const Instruction &instr) {
            int64_t jumpTarget = -1;

            switch (instr.opcode) {
            case OpCode::Adv: {
                getRegister(Register::A) = calcDv(instr.operand);
            } break;
            case OpCode::Bxl: {
                getRegister(Register::B) = getRegister(Register::B) ^ instr.operand;
            } break;
            case OpCode::Bst: {
                auto combo = getComboOperandValue(instr.operand);
                getRegister(Register::B) = combo & 0b111;
            } break;
            case OpCode::Jnz: {
                if (getRegister(Register::A) != 0) {
                    jumpTarget = instr.operand;
                }
            } break;
            case OpCode::Bxc: {
                getRegister(Register::B) ^= getRegister(Register::C);
            } break;
            case OpCode::Out: {
                if (_previousOut) {
                    _writer->write(',');
                }
                auto combo = getComboOperandValue(instr.operand);
                _writer->write((combo & 0b111) + '0');
                _previousOut = true;
            } break;
            case OpCode::Bdv: {
                getRegister(Register::B) = calcDv(instr.operand);
            } break;
            case OpCode::Cdv: {
                getRegister(Register::C) = calcDv(instr.operand);
            } break;
            default:
                throw std::runtime_error("Unsupported opcode encountered.");
            }

            _instructionPointer = jumpTarget != -1 ? jumpTarget : _instructionPointer + 1;
        }

        std::shared_ptr<IWriter> _writer;
        std::array<int64_t, 3> _registers{};
        size_t _instructionPointer{};
        bool _previousOut = false;
        bool _isInterrupted = false;
    };

} // namespace

namespace part1 {
    class ConsoleWriter : public IWriter {
      public:
        void write(char value) override {
            std::cout << value;
        }
    };

    template <typename Input>
    void execute(const Input &input) {
        Processor processor(std::make_shared<ConsoleWriter>());
        processor.setState(input.initialState);
        processor.runProgram(input.program);
    }
} // namespace part1

namespace part2 {

    class OutputMatcher : public IWriter {
      public:
        OutputMatcher(const Program &expectedProgram) : _expectedProgram(expectedProgram) {
        }

        void write(char value) override {
            if (value == ',') {
                return;
            }
            char expectedValue = getExpectedValue();
            if (expectedValue != value) {
                if (_processor) {
                    _processor->setInterrupt(true);
                    return;
                }
            }
            _index++;
        }

        void setProcessor(Processor *processor) {
            if (!processor) {
                throw std::runtime_error("processor cannot be null");
            }
            _processor = processor;
        }

        void reset() {
            _index = 0;
        }

        bool isMatched() const {
            int64_t count = _expectedProgram.instructions.size();
            return _index == count * 2;
        }

      private:
        char getExpectedValue() {
            int64_t instrIndex = static_cast<int64_t>(_index / 2);
            int64_t isOperand = static_cast<bool>(_index % 2);

            auto &instr = _expectedProgram.instructions[instrIndex];
            int64_t value = isOperand ? instr.operand : static_cast<int64_t>(instr.opcode);
            return value + '0';
        }

        const Program &_expectedProgram;
        Processor *_processor;
        int64_t _index{0};
    };

    template <typename Input>
    void execute(const Input &input) {
        InitialState initialState = input.initialState;
        auto matcher = std::make_shared<OutputMatcher>(input.program);
        Processor processor(matcher);
        matcher->setProcessor(&processor);

        for (int64_t regA = 1; regA < std::numeric_limits<int64_t>::max(); regA++) {
            initialState.registers[static_cast<int64_t>(Register::A)] = regA;
            matcher->reset();
            processor.setState(initialState);
            processor.runProgram(input.program);

            if (matcher->isMatched()) {
                std::cout << "\nInitial state: " << initialState.registers << "\n";
                break;
            }
        }
    }
} // namespace part2

int main() {
    auto input = loadInput("day17.txt");
    std::cout << input << std::endl;

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
