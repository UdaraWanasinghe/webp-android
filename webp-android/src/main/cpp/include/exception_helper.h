//
// Created by udara on 6/4/23.
//

#pragma once

#include <string>
#include <cstdarg>

static inline void throwNullPointerException(JNIEnv *env, const char *format, ...);

static inline void throwIllegalArgumentException(JNIEnv *env, const char *format, ...);

static inline void throwArrayIndexOutOfBoundsException(JNIEnv *env, const char *format, ...);

static inline void throwIOException(JNIEnv *env, const char *format, ...);

static inline void throwFileNotFoundException(JNIEnv *env, const char *format, ...);

static inline void throwArithmeticException(JNIEnv *env, const char *format, ...);

static inline void throwUnsupportedOperationException(JNIEnv *env, const char *format, ...);

static inline void throwIllegalStateException(JNIEnv *env, const char *format, ...);

static inline void throwNoSuchElementException(JNIEnv *env, const char *format, ...);

static inline void throwNumberFormatException(JNIEnv *env, const char *format, ...);

static inline void throwRuntimeException(JNIEnv *env, const char *format, ...);