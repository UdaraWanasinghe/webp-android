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
    throwException(env, JavaClass::nullPointerExceptionClass, format, args);
    va_end(args);
}

void throwIllegalArgumentException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::illegalArgumentExceptionClass, format, args);
    va_end(args);
}

void throwArrayIndexOutOfBoundsException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::arrayIndexOutOfBoundsExceptionClass, format, args);
    va_end(args);
}

void throwIOException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::ioExceptionClass, format, args);
    va_end(args);
}

void throwFileNotFoundException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::fileNotFoundExceptionClass, format, args);
    va_end(args);
}

void throwArithmeticException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::arithmeticExceptionClass, format, args);
    va_end(args);
}

void throwUnsupportedOperationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::unsupportedOperationExceptionClass, format, args);
    va_end(args);
}

void throwIllegalStateException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::illegalStateExceptionClass, format, args);
    va_end(args);
}

void throwNoSuchElementException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::noSuchElementExceptionClass, format, args);
    va_end(args);
}

void throwNumberFormatException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::numberFormatExceptionClass, format, args);
    va_end(args);
}

void throwRuntimeException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::runtimeExceptionClass, format, args);
    va_end(args);
}

void throwCancellationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, JavaClass::cancellationExceptionClass, format, args);
    va_end(args);
}

std::string getExceptionMessage(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);

    jthrowable exception = env->ExceptionOccurred();
    env->ExceptionClear();
    jmethodID message_method_id = env->GetMethodID(
            JavaClass::throwableClass,
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