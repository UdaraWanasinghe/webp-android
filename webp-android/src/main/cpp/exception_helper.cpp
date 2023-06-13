//
// Created by udara on 6/4/23.
//

#include <jni.h>

#include "include/string_formatter.h"
#include "include/exception_helper.h"

void throwException(JNIEnv *env, jclass *exception_class, const char *format, ...) {
    va_list args;
    va_start(args, format);

    std::string message = formatString(format, args);
    env->ThrowNew(*exception_class, message.c_str());
    env->DeleteLocalRef(*exception_class);

    va_end(args);
}

void throwNullPointerException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/NullPointerException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwIllegalArgumentException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/IllegalArgumentException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwArrayIndexOutOfBoundsException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwIOException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/io/IOException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwFileNotFoundException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/io/FileNotFoundException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwArithmeticException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/ArithmeticException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwUnsupportedOperationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/UnsupportedOperationException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwIllegalStateException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/IllegalStateException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwNoSuchElementException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/util/NoSuchElementException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwNumberFormatException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/NumberFormatException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

void throwRuntimeException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    jclass exception_class = env->FindClass("java/lang/RuntimeException");
    throwException(env, &exception_class, format, args);
    va_end(args);
}

std::string getExceptionMessage(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);

    jthrowable exception = env->ExceptionOccurred();
    jclass throwable_class = env->FindClass("java/lang/Throwable");
    jmethodID message_method_id = env->GetMethodID(
            throwable_class,
            "getMessage",
            "()Ljava/lang/String;"
    );

    auto message_string = (jstring) env->CallObjectMethod(exception, message_method_id);
    const char *message_chars = env->GetStringUTFChars(message_string, nullptr);
    std::string message = formatString(format, message_chars, args);

    env->DeleteLocalRef(exception);
    env->DeleteLocalRef(throwable_class);
    env->ReleaseStringUTFChars(message_string, message_chars);
    env->DeleteLocalRef(message_string);
    va_end(args);

    return message;
}