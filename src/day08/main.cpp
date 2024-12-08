#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unordered_set>
#include "input.h"
#include "array2d.h"
#include "Coord.h"


namespace {

    using Map = Array2D<char>;
    using Frequency = char;
    using AntennaGroups = std::unordered_map<Frequency, std::vector<Coord>>;

    Map loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        auto map = input::load2D<char>(lines, [](char a) { return a; });
        return map;
    }

    template<typename T>
    std::vector<std::pair<T, T>> makePairs(const std::vector<T> &elements) {
        std::vector<std::pair<T, T>> pairs;
        for (size_t i = 0; i < elements.size(); ++i) {
            for (size_t j = i + 1; j < elements.size(); ++j) {
                pairs.emplace_back(elements[i], elements[j]);
            }
        }
        return pairs;
    }

    AntennaGroups groupAntennas(const Map &map) {
        AntennaGroups groups{};
        for (int row = 0; row < map.rows(); row++) {
            for (int col = 0; col < map.cols(); col++) {
                bool isFrequency = map(row, col) != '.';
                if (!isFrequency) {
                    continue;
                }
                auto frequency = map(row, col);
                groups[frequency].push_back(Coord{.col=col, .row=row});
            }
        }
        return groups;
    }

    std::unordered_set<Coord> findAntinodes(const AntennaGroups &antennaGroups, const Map &map,
                                            std::function<std::vector<Coord>(std::pair<Coord, Coord>,
                                                                             const Map &)> createAntinodesForPairFunc) {
        std::unordered_set<Coord> antinodes{};

        for (const auto &[frequency, coords]: antennaGroups) {
            for (auto const &pair: makePairs(coords)) {
                auto antinodesForPair = createAntinodesForPairFunc(pair, map);
                antinodes.insert(antinodesForPair.begin(), antinodesForPair.end());
            }
        }
        return antinodes;
    }

}

namespace part1 {

    std::vector<Coord> createAntinodes(const std::pair<Coord, Coord> &pair, const Map &map) {
        auto &[coord1, coord2] = pair;
        auto distanceCoord = coord2 - coord1;
        auto antinode1 = coord1 - distanceCoord;
        auto antinode2 = coord2 + distanceCoord;

        std::vector<Coord> antinodes{};
        if (map.isInBounds(antinode1.row, antinode1.col)) {
            antinodes.push_back(antinode1);
        }
        if (map.isInBounds(antinode2.row, antinode2.col)) {
            antinodes.push_back(antinode2);
        }
        return antinodes;
    }

    template<typename Input>
    void execute(Input &map) {
        auto antennaGroups = groupAntennas(map);
        auto antinodes = findAntinodes(antennaGroups, map, createAntinodes);

        std::cout << antinodes.size() << std::endl;
    }
}

namespace part2 {

    std::vector<Coord> createAntinodes(const std::pair<Coord, Coord> &pair, const Map &map) {
        std::vector<Coord> antinodes{};
        auto &[coord1, coord2] = pair;
        auto antinodeDistance = coord2 - coord1;

        auto antinode = coord1;
        while (map.isInBounds(antinode.row, antinode.col)) {
            antinodes.push_back(antinode);
            antinode = antinode - antinodeDistance;
        }

        antinode = coord2;
        while (map.isInBounds(antinode.row, antinode.col)) {
            antinodes.push_back(antinode);
            antinode = antinode + antinodeDistance;
        }

        return antinodes;
    }

    template<typename Input>
    void execute(Input &map) {
        auto antennaGroups = groupAntennas(map);
        auto antinodes = findAntinodes(antennaGroups, map, createAntinodes);

        std::cout << antinodes.size() << std::endl;
    }
}

int main() {
    auto input = loadInput("day08.txt");

    part1::execute(input);
    part2::execute(input);

    return 0;
}
