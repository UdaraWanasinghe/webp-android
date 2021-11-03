//
// Created by udara on 11/3/21.
//

#ifndef WEBP_ANDROID_HELPER_H
#define WEBP_ANDROID_HELPER_H

#endif //WEBP_ANDROID_HELPER_H

#include <jni.h>

static jboolean getBooleanValue(JNIEnv *env, jobject object) {
    jclass cls = env->FindClass("java/lang/Boolean");
    return env->CallBooleanMethod(object, env->GetMethodID(cls, "booleanValue", "()Z"));
}

static jint getIntegerValue(JNIEnv *env, jobject object) {
    jclass cls = env->FindClass("java/lang/Integer");
    return env->CallIntMethod(object, env->GetMethodID(cls, "intValue", "()I"));
}

static jlong getLong(JNIEnv *env, jobject object) {
    jclass cls = env->FindClass("java/lang/Long");
    return env->CallLongMethod(object, env->GetMethodID(cls, "longValue", "()J"));
}

static jfloat getFloat(JNIEnv *env, jobject object) {
    jclass cls = env->FindClass("java/lang/Float");
    return env->CallFloatMethod(object, env->GetMethodID(cls, "floatValue", "()F"));
}