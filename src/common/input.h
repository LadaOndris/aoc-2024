
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
#include <sstream>
#include <cstdint>
#include "array2d.h"


namespace input {

    enum class Blanks {
        Allow,
        Remove
    };


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

    template<typename T>
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

    void ltrim(std::string& s);

    void rtrim(std::string& s);

    void trim(std::string& s);

    std::vector<std::string> split(const std::string &string, char delimiter, Blanks blanksOption = Blanks::Allow);

    template<typename T>
    std::vector<T> convertStringsToNumbers(const std::vector<std::string> &strings) {
        std::vector<T> numbers{};

        for (const auto &str: strings) {
            std::istringstream iss(str);
            T number;
            if (!(iss >> number)) {
                throw std::invalid_argument("Invalid conversion for string: " + str);
            }
            numbers.push_back(number);
        }

        return numbers;
    }

    template<typename T>
    std::vector<T> parseVector(const std::string &string, const char delimiter) {
        auto splits = split(string, delimiter, Blanks::Remove);
        return convertStringsToNumbers<T>(splits);
    }


}

#endif
