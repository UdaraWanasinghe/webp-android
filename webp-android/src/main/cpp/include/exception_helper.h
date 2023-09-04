//
// Created by udara on 6/4/23.
//

#pragma once

#include <string>
#include <cstdarg>

void throwRuntimeException(JNIEnv *env, const char *format, ...);

void throwCancellationException(JNIEnv *env, const char *format, ...);