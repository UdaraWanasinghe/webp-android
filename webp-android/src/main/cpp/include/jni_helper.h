//
// Created by udara on 11/3/21.
//

#pragma once

#include <jni.h>
#include <stdlib.h>
#include <string>

static jboolean GetBooleanValue(JNIEnv *env, jobject object) {
    jclass Boolean = env->FindClass("java/lang/Boolean");
    return env->CallBooleanMethod(object, env->GetMethodID(Boolean, "booleanValue", "()Z"));
}

static jint GetIntegerValue(JNIEnv *env, jobject object) {
    jclass Integer = env->FindClass("java/lang/Integer");
    return env->CallIntMethod(object, env->GetMethodID(Integer, "intValue", "()I"));
}

static jlong GetLongValue(JNIEnv *env, jobject object) {
    jclass Long = env->FindClass("java/lang/Long");
    return env->CallLongMethod(object, env->GetMethodID(Long, "longValue", "()J"));
}

static jfloat GetFloatValue(JNIEnv *env, jobject object) {
    jclass Float = env->FindClass("java/lang/Float");
    return env->CallFloatMethod(object, env->GetMethodID(Float, "floatValue", "()F"));
}

static void ThrowException(JNIEnv *env, std::string message) {
    jclass Exception = env->FindClass("java/lang/Exception");
    env->ThrowNew(Exception, message.data());
}