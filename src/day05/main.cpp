#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include "input.h"

namespace {

    using Update = std::vector<int>;
    using Rule = std::pair<int, int>;

    struct Input {
        std::vector<Rule> rules;
        std::vector<Update> updates;
    };

    Input loadInput(const std::string &filename) {
        Input input{};
        auto fileContent = input::readFile<std::string>(filename, input::read);
        auto parts = input::split(fileContent, "\n\n", input::Blanks::Remove);
        auto rulesPart = parts[0];
        auto updatesPart = parts[1];

        auto rulesLines = input::split(rulesPart, '\n');
        for (const auto &line : rulesLines) {
            auto rule = input::parseVector<int>(line, '|');
            input.rules.emplace_back(rule[0], rule[1]);
        }

        auto updatesLines = input::split(updatesPart, '\n');
        for (const auto &line : updatesLines) {
            auto update = input::parseVector<int>(line, ',');
            input.updates.push_back(update);
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

namespace part1 {

    template<typename Input>
    void execute(Input &input) {
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

namespace part2 {

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

    template<typename Input>
    void execute(Input &input) {
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
    auto input = loadInput("day05.txt");

    part1::execute(input);
    part2::execute(input);

    return 0;
}
