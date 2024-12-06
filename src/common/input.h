
#ifndef AOC_2023_INPUT_H
#define AOC_2023_INPUT_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include "array2d.h"


namespace input {
    void checkStream(const std::ifstream &inputStream);

    std::string read(std::ifstream &inputStream);

    std::string readLine(std::ifstream &inputStream);

    std::vector<std::string> readLines(std::ifstream &inputStream);

    template<typename ReturnType>
    ReturnType readFile(const std::string &fileName, std::function<ReturnType(std::ifstream &)> readerFunction) {
        std::ifstream file(fileName);

        ReturnType result = readerFunction(file);

        file.close();
        return result;
    }

    template <typename T>
    Array2D<T> load2D(const std::vector<std::string> &lines, const std::function<T(char)> &transformFunction) {
        size_t rows = lines.size();
        size_t cols = lines[0].size();
        Array2D<T> array(rows, cols);

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                array(row, col) = transformFunction(lines[row][col]);
            }
        }
        return array;
    }

}

#endif
