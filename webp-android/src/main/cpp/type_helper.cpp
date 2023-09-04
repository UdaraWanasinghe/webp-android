//
// Created by udara on 6/4/23.
//

#include <jni.h>
#include <iostream>

#include "include/type_helper.h"
#include "include/native_loader.h"

int type::getIntegerValue(
        JNIEnv *env,
        jobject object
) {
    if (!env->IsInstanceOf(object, ClassRegistry::integerClass)) {
        throw std::runtime_error("The given object is not of type Integer.");
    }
    jmethodID method_id = env->GetMethodID(ClassRegistry::integerClass, "intValue", "()I");
    jint value = env->CallIntMethod(object, method_id);
    return static_cast<int>(value);
}

bool type::getBooleanValue(
        JNIEnv *env,
        jobject object
) {
    if (!env->IsInstanceOf(object, ClassRegistry::booleanClass)) {
        throw std::runtime_error("The given object is not of type Boolean.");
    }
    jmethodID method_id = env->GetMethodID(ClassRegistry::booleanClass, "booleanValue", "()Z");
    jboolean value = env->CallBooleanMethod(object, method_id);
    return static_cast<bool>(value);
}

float type::getFloatValue(
        JNIEnv *env,
        jobject object
) {
    if (!env->IsInstanceOf(object, ClassRegistry::floatClass)) {
        throw std::runtime_error("The given object is not of type Float.");
    }
    jmethodID method_id = env->GetMethodID(ClassRegistry::floatClass, "floatValue", "()F");
    jfloat value = env->CallFloatMethod(object, method_id);
    return static_cast<float>(value);
}

jobject type::getObjectField(
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

bool type::isObjectNull(JNIEnv *env, jobject obj) {
    if (obj == nullptr) {
        // If the object is already null, return true
        return true;
    }
    // Check if the object is the null reference
    jboolean isNull = env->IsSameObject(obj, nullptr);
    return static_cast<bool>(isNull);
}