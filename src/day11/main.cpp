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
#include <cmath>

namespace {

    std::vector<uint64_t> loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        return input::parseVector<uint64_t>(lines[0], ' ');
    }

}

namespace part1 {

    template<class T>
    int countDigits(T number) {
        int digits = 0;
        if (number < 0)
            digits = 1; // remove this line if '-' counts as a digit
        while (number) {
            number /= 10;
            digits++;
        }
        return digits;
    }

    std::vector<uint64_t> processSingleNumber(const uint64_t &number) {
        std::vector<uint64_t> newNumbers{};
        int numDigits = countDigits(number);
        if (number == 0l) {
            newNumbers.push_back(1l);
        } else if (numDigits % 2 == 0) {
            auto factor = static_cast<uint64_t>(std::pow(10, numDigits / 2));
            auto firstHalf = static_cast<uint64_t>(number / factor);
            auto secondHalf = static_cast<uint64_t>(number % factor);

            newNumbers.push_back(firstHalf);
            newNumbers.push_back(secondHalf);
        } else {
            newNumbers.push_back(number * 2024l);
        }
        return newNumbers;
    }

    std::vector<uint64_t> blink(const std::vector<uint64_t> &numbers) {
        std::vector<uint64_t> newNumbers{};
        for (const auto &number: numbers) {
            auto resultingNumbers = processSingleNumber(number);
            newNumbers.insert(newNumbers.end(), resultingNumbers.begin(), resultingNumbers.end());
        }
        return newNumbers;
    }

    template<typename Input>
    void execute(const Input &input) {
        std::vector<uint64_t> numbers = input;

        for (int i = 0; i < 25; i++) {
            numbers = blink(numbers);
        }

        std::cout << numbers.size() << std::endl;
    }
}

namespace part2 {

    std::unordered_map<uint64_t, uint64_t> blink(const std::unordered_map<uint64_t, uint64_t> &stoneCounts) {
        std::unordered_map<uint64_t, uint64_t> newCounts{};
        for (const auto &[number, count]: stoneCounts) {
            auto resultingNumbers = part1::processSingleNumber(number);

            for (const auto &resultingNumber: resultingNumbers) {
                newCounts[resultingNumber] += count;
            }
        }
        return newCounts;
    }

    template<typename Input>
    void execute(const Input &input) {
        std::unordered_map<uint64_t, uint64_t> stoneCounts{};
        for (const auto &number: input) {
            stoneCounts[number] += 1l;
        }

        for (int i = 0; i < 75; i++) {
            stoneCounts = blink(stoneCounts);
        }

        auto totalCount = std::accumulate(stoneCounts.begin(), stoneCounts.end(), 0l,
                                          [](uint64_t sum, const std::pair<uint64_t, uint64_t> &pair) {
                                              return sum + pair.second;
                                          });

        std::cout << totalCount << std::endl;
    }
}

int main() {
    auto input = loadInput("day11.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
