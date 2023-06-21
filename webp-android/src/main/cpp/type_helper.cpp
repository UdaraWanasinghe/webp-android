//
// Created by udara on 6/4/23.
//

#include <jni.h>
#include <iostream>

#include "include/type_helper.h"

int getIntegerValue(
        JNIEnv *env,
        jobject object
) {
    jclass clazz = env->FindClass("java/lang/Integer");
    if (!env->IsInstanceOf(object, clazz)) {
        throw std::runtime_error("The given object is not of type Integer.");
    }
    jmethodID method_id = env->GetMethodID(clazz, "intValue", "()I");
    jint value = env->CallIntMethod(object, method_id);
    env->DeleteLocalRef(clazz);
    return static_cast<int>(value);
}

bool getBooleanValue(
        JNIEnv *env,
        jobject object
) {
    jclass clazz = env->FindClass("java/lang/Boolean");
    if (!env->IsInstanceOf(object, clazz)) {
        throw std::runtime_error("The given object is not of type Boolean.");
    }
    jmethodID method_id = env->GetMethodID(clazz, "booleanValue", "()Z");
    jboolean value = env->CallBooleanMethod(object, method_id);
    env->DeleteLocalRef(clazz);
    return static_cast<bool>(value);
}

long getLongValue(
        JNIEnv *env,
        jobject object
) {
    jclass clazz = env->FindClass("java/lang/Long");
    if (!env->IsInstanceOf(object, clazz)) {
        throw std::runtime_error("The given object is not of type Long.");
    }
    jmethodID method_id = env->GetMethodID(clazz, "longValue", "()J");
    jlong value = env->CallLongMethod(object, method_id);
    env->DeleteLocalRef(clazz);
    return static_cast<long>(value);
}

float getFloatValue(
        JNIEnv *env,
        jobject object
) {
    jclass clazz = env->FindClass("java/lang/Float");
    if (!env->IsInstanceOf(object, clazz)) {
        throw std::runtime_error("The given object is not of type Float.");
    }
    jmethodID method_id = env->GetMethodID(clazz, "floatValue", "()F");
    jfloat value = env->CallFloatMethod(object, method_id);
    env->DeleteLocalRef(clazz);
    return static_cast<float>(value);
}

jobject getObjectField(
        JNIEnv *env,
        jobject object,
        const char *name,
        const char *sig
) {
    jclass clazz = env->GetObjectClass(object);
    jfieldID field_id = env->GetFieldID(clazz, name, sig);
    jobject field = env->GetObjectField(object, field_id);
    env->DeleteLocalRef(clazz);
    return field;
}

bool isObjectNull(JNIEnv *env, jobject obj) {
    if (obj == nullptr) {
        // If the object is already null, return true
        return true;
    }

    // Check if the object is the null reference
    jboolean isNull = env->IsSameObject(obj, nullptr);
    return static_cast<bool>(isNull);
}

std::string jstringToString(JNIEnv *env, jstring jstr) {
    const char *chars = env->GetStringUTFChars(jstr, nullptr);
    std::string str(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return str;
}