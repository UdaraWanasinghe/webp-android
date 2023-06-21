//
// Created by udara on 6/4/23.
//

#pragma once

#include <string>
#include <cstdarg>

void throwNullPointerException(JNIEnv *env, const char *format, ...);

void throwIllegalArgumentException(JNIEnv *env, const char *format, ...);

void throwArrayIndexOutOfBoundsException(JNIEnv *env, const char *format, ...);

void throwIOException(JNIEnv *env, const char *format, ...);

void throwFileNotFoundException(JNIEnv *env, const char *format, ...);

void throwArithmeticException(JNIEnv *env, const char *format, ...);

void throwUnsupportedOperationException(JNIEnv *env, const char *format, ...);

void throwIllegalStateException(JNIEnv *env, const char *format, ...);

void throwNoSuchElementException(JNIEnv *env, const char *format, ...);

void throwNumberFormatException(JNIEnv *env, const char *format, ...);

void throwRuntimeException(JNIEnv *env, const char *format, ...);

void throwCancellationException(JNIEnv *env, const char *format, ...);

/**
 * Retrieves an exception message by formatting the given format string and optional arguments.
 * The function takes a JNI environment pointer and a format string along with optional arguments.
 * It returns a C++ std::string containing the formatted exception message.
 *
 * @param env The JNI environment pointer.
 * @param format The format string for the exception message. It can include format specifiers.
 * Must specify additional format "%s" to format message.
 * @param ... Optional arguments to be formatted into the exception message.
 *
 * @return A std::string representing the formatted exception message.
 */
std::string getExceptionMessage(JNIEnv *env, const char *format, ...);