//
// Created by udara on 6/4/23.
//

#include <jni.h>

#include "include/string_formatter.h"
#include "include/exception_helper.h"
#include "include/native_loader.h"

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