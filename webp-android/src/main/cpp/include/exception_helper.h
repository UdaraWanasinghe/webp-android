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