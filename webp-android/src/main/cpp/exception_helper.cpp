//
// Created by udara on 6/4/23.
//

#include <jni.h>

#include "include/string_formatter.h"
#include "include/exception_helper.h"
#include "include/native_helper.h"

namespace {

    void throwException(JNIEnv *env, jclass exception_class, const char *format, ...) {
        va_list args;
        va_start(args, format);

        std::string message = str::formatString(format, args);
        env->ThrowNew(exception_class, message.c_str());
        env->DeleteLocalRef(exception_class);

        va_end(args);
    }

}

void throwNullPointerException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::nullPointerExceptionClass, format, args);
    va_end(args);
}

void throwIllegalArgumentException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::illegalArgumentExceptionClass, format, args);
    va_end(args);
}

void throwArrayIndexOutOfBoundsException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::arrayIndexOutOfBoundsExceptionClass, format, args);
    va_end(args);
}

void throwIOException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::ioExceptionClass, format, args);
    va_end(args);
}

void throwFileNotFoundException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::fileNotFoundExceptionClass, format, args);
    va_end(args);
}

void throwArithmeticException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::arithmeticExceptionClass, format, args);
    va_end(args);
}

void throwUnsupportedOperationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::unsupportedOperationExceptionClass, format, args);
    va_end(args);
}

void throwIllegalStateException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::illegalStateExceptionClass, format, args);
    va_end(args);
}

void throwNoSuchElementException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::noSuchElementExceptionClass, format, args);
    va_end(args);
}

void throwNumberFormatException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::numberFormatExceptionClass, format, args);
    va_end(args);
}

void throwRuntimeException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::runtimeExceptionClass, format, args);
    va_end(args);
}

void throwCancellationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::cancellationExceptionClass, format, args);
    va_end(args);
}

std::string getExceptionMessage(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);

    jthrowable exception = env->ExceptionOccurred();
    env->ExceptionClear();
    jmethodID message_method_id = env->GetMethodID(
            ClassRegistry::throwableClass,
            "getMessage",
            "()Ljava/lang/String;"
    );

    auto message_string = (jstring) env->CallObjectMethod(exception, message_method_id);
    const char *message_chars = env->GetStringUTFChars(message_string, nullptr);
    std::string message = str::formatString(format, message_chars, args);

    env->DeleteLocalRef(exception);
    env->ReleaseStringUTFChars(message_string, message_chars);
    env->DeleteLocalRef(message_string);
    va_end(args);

    return message;
}