#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include "input.h"


namespace {
    struct Input {
        explicit Input(std::size_t size) {
            firstList.reserve(size);
            secondList.reserve(size);
        }

        std::vector<int> firstList;
        std::vector<int> secondList;
    };

    Input loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);

        Input input(lines.size());

        int firstNumber;
        int secondNumber;

        for (const auto &line: lines) {
            std::istringstream iss(line);
            iss >> firstNumber >> secondNumber;

            input.firstList.push_back(firstNumber);
            input.secondList.push_back(secondNumber);
        }
        return input;
    }

}

namespace part1 {

    void sortList(std::vector<int> &list) {
        std::sort(list.begin(), list.end());
    }

    std::vector<int> compareItems(const std::vector<int> &firstList, const std::vector<int> &secondList) {
        std::vector<int> result(firstList.size());
        std::transform(firstList.begin(), firstList.end(),
                       secondList.begin(), result.begin(),
                       [](int a, int b) { return std::abs(a - b); });
        return result;
    }

    template<typename Input>
    void execute(Input &input) {
        sortList(input.firstList);
        sortList(input.secondList);

        auto result = compareItems(input.firstList, input.secondList);

        auto sum = std::accumulate(result.begin(), result.end(), 0);
        std::cout << sum << std::endl;
    }
}

namespace part2 {

    std::vector<int> computeSimilarityScores(const std::vector<int> &list, std::unordered_map<int, int> &counts) {
        std::vector<int> result{};
        result.reserve(list.size());

        for (const auto &item: list) {
            int similarityScore = item * counts[item];
            result.push_back(similarityScore);
        }
        return result;
    }

    template<typename Input>
    void execute(Input &input) {
        std::unordered_map<int, int> counts;
        for (const auto &item: input.secondList) {
            counts[item] += 1;
        }

        std::vector<int> similarityScores = computeSimilarityScores(input.firstList, counts);

        auto sum = std::accumulate(similarityScores.begin(), similarityScores.end(), 0);
        std::cout << sum << std::endl;
    }
}

int main() {
    auto input = loadInput("day01.txt");

    part1::execute(input);
    part2::execute(input);

    return 0;
}
