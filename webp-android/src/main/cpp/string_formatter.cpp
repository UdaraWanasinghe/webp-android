//
// Created by udara on 6/4/23.
//

#include <string>
#include <cstdarg>
#include <cstdio>

#include "include/string_formatter.h"

static inline std::string formatString(const char *format, ...) {
    // Create a va_list to hold the variable arguments
    va_list args;
    va_start(args, format);

    // Determine the length of the formatted string
    va_list argsCopy;
    va_copy(argsCopy, args);
    int length = std::vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);

    // Create a buffer of sufficient length
    std::string buffer(length + 1, '\0');

    // Format the string using vsnprintf
    std::vsnprintf(&buffer[0], buffer.size(), format, args);

    // Clean up the va_list
    va_end(args);

    // Resize the buffer to exclude the null terminator
    buffer.resize(length);

    return buffer;
}