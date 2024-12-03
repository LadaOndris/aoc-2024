
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
}

#endif
