#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "input.h"

namespace {

    struct Report {
        std::vector<int> levels;

        friend std::ostream &operator<<(std::ostream &out, const Report &report) {
            out << "[";
            for (size_t i = 0; i < report.levels.size(); i++) {
                out << report.levels[i];
                if (i < report.levels.size() - 1) {
                    out << ",";
                }
            }
            out << "]\n";
            return out;
        }
    };

    struct Input {
        std::vector<Report> reports;

        friend std::ostream &operator<<(std::ostream &out, const Input &input) {
            out << "[";
            for (size_t i = 0; i < input.reports.size(); i++) {
                out << input.reports[i];
                if (i < input.reports.size() - 1) {
                    out << ",";
                }
            }
            out << "]";
            return out;
        }
    };

    Input loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        Input input{};

        for (const auto &line: lines) {
            auto report = input::parseVector<int>(line, ' ');
            input.reports.emplace_back(report);
        }

        return input;
    }

    int evaluatePredicate(int stride, const Report &report,
                          const std::function<bool(int currentLevel, int nextLevel)>& predicate) {
        const auto &levels = report.levels;

        for (size_t i = 0; i < levels.size() - stride; i += stride) {
            bool conditionFailed = !predicate(levels[i], levels[i + stride]);
            if (conditionFailed) {
                return false;
            }
        }

        return true;
    }

    bool isIncreasingOrDecreasing(int stride, const Report &report) {
        auto isIncreasingPredicate = [](int currentLevel, int nextLevel) {
            return currentLevel < nextLevel;
        };

        auto isDecreasingPredicate = [](int currentLevel, int nextLevel) {
            return currentLevel > nextLevel;
        };

        bool isIncreasing = evaluatePredicate(stride, report, isIncreasingPredicate);
        bool isDecreasing = evaluatePredicate(stride, report, isDecreasingPredicate);

        return isIncreasing || isDecreasing;
    }

    bool isDifferenceWithinBounds(int stride, int min, int max, const Report &report) {
        auto predicate = [min, max](int currentLevel, int nextLevel) {
            int diff = std::abs(nextLevel - currentLevel);
            return diff >= min && diff <= max;
        };

        bool isWithinBounds = evaluatePredicate(stride, report, predicate);
        return isWithinBounds;
    }

    bool evaluateRules(const Report &report) {
        int stride = 1;
        int minDifference = 1;
        int maxDifference = 3;

        auto rule1 = [&]() {
            return isIncreasingOrDecreasing(stride, report);
        };
        auto rule2 = [&]() {
            return isDifferenceWithinBounds(stride, minDifference, maxDifference, report);
        };

        std::vector<std::function<bool()>> rules = {rule1, rule2};

        bool result = std::all_of(rules.begin(), rules.end(), [](const std::function<bool()> &rule) {
            return rule();
        });
        return result;
    }

}

namespace part1 {

    bool isReportSafe(const Report &report) {
        bool result = evaluateRules(report);
        return result;
    }

    template <typename Input>
    void execute(Input &input) {
        auto numSafeReports = std::count_if(input.reports.begin(), input.reports.end(), isReportSafe);
        std::cout << numSafeReports << std::endl;
    }
}

namespace part2 {

    bool isReportSafe(const Report &report) {
        bool result = evaluateRules(report);
        if (result) {
            return true;
        }
        // Evaluate report without a level
        for (int i = 0; i < report.levels.size(); i++) {
            Report reportWithoutSingleLevel = report;
            reportWithoutSingleLevel.levels.erase(reportWithoutSingleLevel.levels.begin() + i);

            result = evaluateRules(reportWithoutSingleLevel);
            if (result) {
                return true;
            }
        }
        return false;
    }

    template <typename Input>
    void execute(Input &input) {
        auto numSafeReports = std::count_if(input.reports.begin(), input.reports.end(), isReportSafe);
        std::cout << numSafeReports << std::endl;
    }
}

int main() {
    auto input = loadInput("day02.txt");

    part1::execute(input);
    part2::execute(input);

    return 0;
}
