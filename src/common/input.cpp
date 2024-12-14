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

    std::vector<std::string> split(const std::string &string, const std::string &delimiter, Blanks blanksOption) {
        std::vector<std::string> parts{};
        size_t start = 0;
        size_t end = string.find(delimiter);

        while (end != std::string::npos) {
            std::string part = string.substr(start, end - start);
            if (blanksOption != Blanks::Remove || !part.empty()) {
                parts.push_back(part);
            }
            start = end + delimiter.length();
            end = string.find(delimiter, start);
        }

        std::string part = string.substr(start);
        if (blanksOption != Blanks::Remove || !part.empty()) {
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

    std::vector<std::string> parseVector(const std::string &input, const std::string &pattern) {
        std::vector<std::string> matches{};

        std::regex regex(pattern);
        std::sregex_iterator begin(input.begin(), input.end(), regex);
        std::sregex_iterator end{};

        for (auto it = begin; it != end; ++it) {
            // Skip the full match at index 0
            for (size_t i = 1; i < it->size(); ++i) {
                matches.push_back((*it)[i].str());
            }
        }
        return matches;
    }

}