//
// Created by lada on 4/7/24.
//

#include "input.h"

namespace input {
    void checkStream(const std::ifstream &inputStream) {
        if (!inputStream.is_open()) {
            throw std::ios_base::failure("Input stream is not open.");
        }
    }

    std::string readLine(std::ifstream &inputStream) {
        checkStream(inputStream);

        std::string line;
        std::getline(inputStream, line);
        return line;
    }

    std::vector<std::string> readLines(std::ifstream &inputStream) {
        checkStream(inputStream);

        std::vector<std::string> lines{};
        std::string line;
        while (std::getline(inputStream, line)) {
            lines.push_back(line);
        }

        return lines;
    }
}