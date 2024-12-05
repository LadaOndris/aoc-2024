#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <optional>
#include <unordered_set>
#include "input.h"
#include "print.h"
#include "array2d.h"


namespace {

    using Update = std::vector<int>;
    using Rule = std::pair<int, int>;

    struct Input {
        std::vector<Rule> rules;
        std::vector<Update> updates;
    };

    Input loadInput(std::vector<std::string> &lines) {
        Input input{};

        bool parsingRules = true;
        for (const auto &line: lines) {
            if (line.empty()) {
                parsingRules = false;
                continue;
            }

            std::stringstream ss(line);

            if (parsingRules) {
                std::string firstPart, secondPart;
                if (std::getline(ss, firstPart, '|') && std::getline(ss, secondPart)) {
                    int num1 = std::stoi(firstPart);
                    int num2 = std::stoi(secondPart);
                    input.rules.emplace_back(num1, num2);
                }
            } else {
                Update update{};
                std::string number;
                while (std::getline(ss, number, ',')) {
                    update.push_back(std::stoi(number));
                }
                input.updates.push_back(update);
            }
        }
        return input;
    }

    using RuleMap = std::unordered_map<int, std::vector<Rule>>;

    RuleMap formRuleMap(const std::vector<Rule> &rules) {
        // I need to form a std::unordered_map<int, std::vector<Rule>> containing the rules grouped by the right side of the rule
        RuleMap map{};
        for (const auto &rule: rules) {
            map[rule.second].push_back(rule);
        }
        return map;
    }

    bool existsRuleForThatNumber(int number, const std::vector<Rule> &matchingRules) {
        auto ruleMatchesNumber = [number](const Rule &rule) {
            return rule.first == number;
        };
        return std::any_of(matchingRules.begin(), matchingRules.end(), ruleMatchesNumber);
    }

    bool isUpdateValid(const Update &update, RuleMap &ruleMap) {
        // I will go through the update numbers one by one
        for (int currentNumberId = 0; currentNumberId < update.size(); currentNumberId++) {
            int number = update[currentNumberId];
            auto matchingRules = ruleMap[number];
            // And check that for each number on the left exists a rule
            for (int leftOfCurrentId = 0; leftOfCurrentId < currentNumberId; leftOfCurrentId++) {
                int leftOfNumber = update[leftOfCurrentId];
                bool ruleExists = existsRuleForThatNumber(leftOfNumber, matchingRules);
                if (!ruleExists) {
                    return false;
                }
            }
        }
        return true;
    }

    int getMiddleValue(const Update &update) {
        int size = static_cast<int>(update.size());
        int index = size / 2; // Assumption is that there is an odd count of numbers.
        return update[index];
    }

}

namespace Part1 {

    void execute(std::vector<std::string> &lines) {
        auto input = loadInput(lines);

//        std::cout << input.rules << std::endl;
//        std::cout << input.updates << std::endl;

        auto ruleMap = formRuleMap(input.rules);

        int sumOfMiddleValues = 0;
        for (const auto &update: input.updates) {
            bool isValid = isUpdateValid(update, ruleMap);
            if (isValid) {
                sumOfMiddleValues += getMiddleValue(update);
            }

        }
        std::cout << sumOfMiddleValues << std::endl;
    }
}

namespace Part2 {

    Update reorderUpdate(Update &update, RuleMap &ruleMap) {
        for (int currentNumberId = 0; currentNumberId < update.size(); currentNumberId++) {
            int number = update[currentNumberId];
            auto matchingRules = ruleMap[number];

            // Check all numbers to the left of the current number
            for (int leftOfCurrentId = 0; leftOfCurrentId < currentNumberId; leftOfCurrentId++) {
                int leftOfNumber = update[leftOfCurrentId];

                if (!existsRuleForThatNumber(leftOfNumber, matchingRules)) {
                    // Swap the two numbers
                    std::swap(update[leftOfCurrentId], update[currentNumberId]);
                    // Recursively keep swapping numbers that do not form a match
                    return reorderUpdate(update, ruleMap);
                }
            }
        }
        return update;
    }

    void execute(std::vector<std::string> &lines) {
        auto input = loadInput(lines);

//        std::cout << input.rules << std::endl;
//        std::cout << input.updates << std::endl;

        auto ruleMap = formRuleMap(input.rules);

        int sumOfMiddleValues = 0;
        for (auto &update: input.updates) {
            bool isValid = isUpdateValid(update, ruleMap);
            if (!isValid) {
                auto reorderedUpdate = reorderUpdate(update, ruleMap);
                sumOfMiddleValues += getMiddleValue(reorderedUpdate);
            }

        }
        std::cout << sumOfMiddleValues << std::endl;
    }
}

int main() {
    auto lines = input::readFile<std::vector<std::string>>("day05.txt", input::readLines);

    Part1::execute(lines);
    Part2::execute(lines);

    return 0;
}
