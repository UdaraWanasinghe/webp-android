//
// Created by udara on 6/4/23.
//

#include "include/exception_helper.h"
#include "include/native_loader.h"
#include "include/string_formatter.h"

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

void exc::throwRuntimeException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::runtimeExceptionClass.get(env), format, args);
    va_end(args);
}

void exc::throwCancellationException(JNIEnv *env, const char *format, ...) {
    va_list args;
    va_start(args, format);
    throwException(env, ClassRegistry::cancellationExceptionClass.get(env), format, args);
    va_end(args);
}