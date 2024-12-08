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

    std::string read(std::ifstream &inputStream) {
        checkStream(inputStream);

        std::string content((std::istreambuf_iterator<char>(inputStream)),
                            std::istreambuf_iterator<char>());
        return content;
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

    std::vector<std::string> split(const std::string &string, char delimiter, Blanks blanksOption) {
        std::vector<std::string> parts{};
        std::istringstream iss(string);
        std::string part{};

        while (std::getline(iss, part, delimiter)) {
            if (blanksOption == Blanks::Remove) {
                if (part.empty()) {
                    continue; // Skip empty string
                }
            }
            parts.push_back(part);
        }
        return parts;
    }

    void ltrim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](unsigned char ch) { return !std::isspace(ch); }));
    }

    void rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }

    void trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
    }

}