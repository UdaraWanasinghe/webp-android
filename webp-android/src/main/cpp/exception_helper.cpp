//
// Created by udara on 6/4/23.
//

#include <jni.h>

#include "include/string_formatter.h"
#include "include/exception_helper.h"

void throwException(JNIEnv *env, jclass *exceptionClass, const char *format, ...) {
    va_list args;
    va_start(args, format);

    std::string message = formatString(format, args);
    env->ThrowNew(*exceptionClass, message.c_str());
    env->DeleteLocalRef(*exceptionClass);

    va_end(args);
}

void throwNullPointerException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/NullPointerException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwIllegalArgumentException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/IllegalArgumentException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwArrayIndexOutOfBoundsException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwIOException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/io/IOException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwFileNotFoundException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/io/FileNotFoundException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwArithmeticException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/ArithmeticException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwUnsupportedOperationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/UnsupportedOperationException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwIllegalStateException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/IllegalStateException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwNoSuchElementException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/util/NoSuchElementException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwNumberFormatException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/NumberFormatException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

void throwRuntimeException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
    throwException(env, &exceptionClass, format, args);
    va_end(args);
}

std::string getExceptionMessage(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);

    jthrowable exception = env->ExceptionOccurred();
    jclass throwableClass = env->FindClass("java/lang/Throwable");
    jmethodID messageMethodID = env->GetMethodID(
            throwableClass,
            "getMessage",
            "()Ljava/lang/String;"
    );

    auto messageString = (jstring) env->CallObjectMethod(exception, messageMethodID);
    const char *messageChars = env->GetStringUTFChars(messageString, nullptr);
    std::string message = formatString(format, messageChars, args);

    env->DeleteLocalRef(exception);
    env->DeleteLocalRef(throwableClass);
    env->ReleaseStringUTFChars(messageString, messageChars);
    env->DeleteLocalRef(messageString);
    va_end(args);

    return message;
}