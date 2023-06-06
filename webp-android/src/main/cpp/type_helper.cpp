//
// Created by udara on 6/4/23.
//

#include <jni.h>
#include <iostream>

#include "include/type_helper.h"

int getIntegerValue(JNIEnv *env, jobject *object) {
    jclass integerClass = env->FindClass("java/lang/Integer");
    if (!env->IsInstanceOf(*object, integerClass)) {
        throw std::runtime_error("The given object is not of type Integer.");
    }
    jmethodID intValueMethod = env->GetMethodID(integerClass, "intValue", "()I");
    jint value = env->CallIntMethod(*object, intValueMethod);
    env->DeleteLocalRef(integerClass);
    return static_cast<int>(value);
}

bool getBooleanValue(JNIEnv *env, jobject *object) {
    jclass booleanClass = env->FindClass("java/lang/Boolean");
    if (!env->IsInstanceOf(*object, booleanClass)) {
        throw std::runtime_error("The given object is not of type Boolean.");
    }
    jmethodID booleanValueMethod = env->GetMethodID(booleanClass, "booleanValue", "()Z");
    jboolean value = env->CallBooleanMethod(*object, booleanValueMethod);
    env->DeleteLocalRef(booleanClass);
    return static_cast<bool>(value);
}

long getLongValue(JNIEnv *env, jobject *object) {
    jclass longClass = env->FindClass("java/lang/Long");
    if (!env->IsInstanceOf(*object, longClass)) {
        throw std::runtime_error("The given object is not of type Long.");
    }
    jmethodID longValueMethod = env->GetMethodID(longClass, "longValue", "()J");
    jlong value = env->CallLongMethod(*object, longValueMethod);
    env->DeleteLocalRef(longClass);
    return static_cast<long>(value);
}

float getFloatValue(JNIEnv *env, jobject *object) {
    jclass floatClass = env->FindClass("java/lang/Float");
    if (!env->IsInstanceOf(*object, floatClass)) {
        throw std::runtime_error("The given object is not of type Float.");
    }
    jmethodID floatValueMethod = env->GetMethodID(floatClass, "floatValue", "()F");
    jfloat value = env->CallFloatMethod(*object, floatValueMethod);
    env->DeleteLocalRef(floatClass);
    return static_cast<float>(value);
}

jobject getObjectField(
        JNIEnv *env,
        jobject *object,
        const char *name,
        const char *sig
) {
    jclass objectClass = env->GetObjectClass(*object);
    jfieldID fieldID = env->GetFieldID(objectClass, name, sig);
    jobject field = env->GetObjectField(*object, fieldID);
    env->DeleteLocalRef(objectClass);
    return field;
}