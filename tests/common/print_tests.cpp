
#include "print.h"
#include <gtest/gtest.h>
#include <ostream>
#include <string_view>

using namespace std::literals::string_view_literals;

TEST(PrintVector, PrintsIntVector) {
    std::vector<int> data{1, 2, 3};

    std::ostringstream result{};

    result << data;

    auto output{static_cast<std::string>(result.str())};

    EXPECT_STREQ(output.c_str(), "[1,2,3]");
}


TEST(PrintVector, PrintsUint8Vector) {
    std::vector<uint8_t> data{1, 2, 3};

    std::ostringstream result{};

    result << data;

    auto output{static_cast<std::string>(result.str())};

    EXPECT_STREQ(output.c_str(), "[1,2,3]");
}


TEST(PrintVector, PrintsStringVector) {
    std::vector<std::string> data{"foo", "bar", "baz"};

    std::ostringstream result{};

    result << data;

    auto output{static_cast<std::string>(result.str())};

    EXPECT_STREQ(output.c_str(), "[\"foo\",\"bar\",\"baz\"]");
}


struct CustomType {
    explicit CustomType(std::string_view name) : name(name) {}

    friend std::ostream &operator<<(std::ostream &out, const CustomType &object) {
        out << "CustomType(name=\"" << object.name << "\")";
        return out;
    }

    std::string name;
};

TEST(PrintVector, PrintsVectorOfCustomType) {
    std::vector<CustomType> data{
            CustomType{"foo"sv},
            CustomType{"bar"sv},
            CustomType{"baz"sv}
    };

    std::ostringstream result{};

    result << data;

    auto output{static_cast<std::string>(result.str())};

    EXPECT_STREQ(output.c_str(), "[CustomType(name=\"foo\"),"
                                 "CustomType(name=\"bar\"),"
                                 "CustomType(name=\"baz\")]");
}