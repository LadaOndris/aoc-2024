
#ifndef AOC_2023_PRINT_H
#define AOC_2023_PRINT_H

#include <vector>
#include <sstream>
#include "array2d.h"

template<typename T>
inline std::string toString(const T &elem) {
    if constexpr (std::is_integral_v<T>) {
        return std::to_string(elem);
    } else {
        std::ostringstream out;
        out << elem;
        return out.str();
    }
}


template<typename T>
inline std::string toString(const std::pair<T, T> &pair) {
    std::ostringstream out;
    out << "(" << pair.first << "," << pair.second << ")";
    return out.str();

}

template<>
inline std::string toString(const std::string &elem) {
    return "\"" + elem + "\"";
}

template<typename T>
inline std::ostream &operator<<(std::ostream &out, const std::pair<T, T> &pair) {
    out << "(" << pair.first << "," << pair.second << ")";
    return out;
}

template<typename T>
inline std::ostream &operator<<(std::ostream &out, const std::vector<T> &data) {
    out << "[";
    for (int i = 0; i < data.size() - 1; i++) {
        out << data[i] << ",";
    }
    out << data[data.size() - 1];
    out << "]";
    return out;
}

template<typename T>
inline std::ostream &operator<<(std::ostream &out, const Array2D<T> &data) {
    out << '[' << std::endl;
    for (int row = 0; row < data.rows(); row++) {
        out << '[';
        for (int col = 0; col < data.cols() - 1; col++) {
            out << data(row, col) << ',';
        }
        out << data(row, data.cols() - 1);
        out << ']' << std::endl;
    }
    out << ']';
    return out;
}


#endif
