//
// Created by udara on 6/4/23.
//

#pragma once

#include <string>

namespace str {
    /**
     * Formats a string based on a format string and variable arguments.
     *
     * This function takes a format string and additional variable arguments, similar to the printf function in C,
     * and returns a formatted string.
     *
     * @param format A pointer to a constant character array (C-string) that specifies the format of the resulting string.
     *               It contains format specifiers that define how the additional arguments will be converted and inserted into the string.
     * @param ... The variable number of arguments that will be formatted and inserted into the resulting string based on the format string.
     *
     * @return A C++ string object that contains the formatted string.
     *
     * @note The function internally uses the variadic function feature of C/C++ to handle a variable number of arguments.
     *       It is important to ensure that the format string and the additional arguments are properly matched to avoid undefined behavior or runtime errors.
     */
    std::string formatString(const char *format, ...);
}