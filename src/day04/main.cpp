#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include "input.h"
#include "print.h"
#include "array2d.h"


namespace {

    Array2D<char> loadInput(std::vector<std::string> &lines) {
        int rows = static_cast<int>(lines.size());
        int cols = static_cast<int>(lines[0].size());

        Array2D<char> array(rows, cols);

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                array(row, col) = lines[row][col];
            }
        }
        return array;
    }

}

namespace Part1 {

    Array2D<char> rotate90(const Array2D<char> &array) {
        Array2D<char> rotated(array.cols(), array.rows());

        for (size_t row = 0; row < array.rows(); ++row) {
            for (size_t col = 0; col < array.cols(); ++col) {
                rotated(col, array.rows() - 1 - row) = array(row, col);
            }
        }
        return rotated;
    }

    int findStringInRows(Array2D<char> &array, const std::string &pattern) {
        int count = 0;
        int wordLen = pattern.size();
        for (size_t row = 0; row < array.rows(); ++row) {
            auto rowIter = array.rowBegin(row);
            for (size_t col = 0; col + wordLen <= array.cols(); ++col) {
                if (std::equal(pattern.begin(), pattern.end(), rowIter + col)) {
                    ++count;
                }
            }
        }
        return count;
    }

    int findStringDiagonally(const Array2D<char> &array, const std::string &pattern) {
        int count = 0;
        // Start at each position in the array
        for (size_t row = 0; row < array.rows(); ++row) {
            for (size_t col = 0; col < array.cols(); ++col) {
                // Check each letter in the pattern in the right direction if in bounds
                for (int i = 0; i < pattern.size(); i++) {
                    // Diagonally shifted coordinates
                    int shiftedRow = row + i;
                    int shiftedCol = col + i;

                    if (!array.isInBounds(shiftedRow, shiftedCol)) {
                        break;
                    }
                    // Check characters match
                    if (array(shiftedRow, shiftedCol) != pattern[i]) {
                        break;
                    }
                    // Match found
                    if (i == pattern.size() - 1) {
                        ++count;
                    }
                }

            }
        }
        return count;
    }

    void execute(std::vector<std::string> &lines) {
        auto array0deg = loadInput(lines);
        auto array90deg = rotate90(array0deg);
        auto array180deg = rotate90(array90deg);
        auto array270deg = rotate90(array180deg);

        std::array<int, 8> counts{};
        counts[0] = findStringInRows(array0deg, "XMAS");
        counts[1] = findStringInRows(array90deg, "XMAS");
        counts[2] = findStringInRows(array180deg, "XMAS");
        counts[3] = findStringInRows(array270deg, "XMAS");

        counts[4] = findStringDiagonally(array0deg, "XMAS");
        counts[5] = findStringDiagonally(array90deg, "XMAS");
        counts[6] = findStringDiagonally(array180deg, "XMAS");
        counts[7] = findStringDiagonally(array270deg, "XMAS");

        int totalCount = std::accumulate(counts.begin(), counts.end(), 0);

        std::cout << totalCount << std::endl;
    }
}

namespace Part2 {

    bool areCornerCharsValid(std::array<char, 4> &corners) {
        // The corners form opposing MAS. MAM or SAS is not allowed.
        bool opposingCorners = corners[0] != corners[2] && corners[1] != corners[3];

        std::sort(corners.begin(), corners.end());
        bool hasTwoPairs = corners[0] == corners[1] && corners[2] == corners[3];

        bool correctLetters = corners[0] == 'M' && corners[2] == 'S';

        return opposingCorners && hasTwoPairs && correctLetters;
    }

    int searchForCrossMas(const Array2D<char> &array) {
        std::array<char, 4> corners{};
        int count = 0;

        for (size_t row = 1; row < array.rows() - 1; ++row) {
            for (size_t col = 1; col < array.cols() - 1; ++col) {
                if (array(row, col) == 'A') {
                    corners[0] = array(row - 1, col - 1);
                    corners[1] = array(row - 1, col + 1);
                    corners[2] = array(row + 1, col + 1);
                    corners[3] = array(row + 1, col - 1);

                    if (areCornerCharsValid(corners)) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    void execute(std::vector<std::string> &lines) {
        auto array = loadInput(lines);
        int count = searchForCrossMas(array);

        std::cout << count << std::endl;
    }
}

int main() {
    auto lines = input::readFile<std::vector<std::string>>("day04.txt", input::readLines);

    Part1::execute(lines);
    Part2::execute(lines);

    return 0;
}
