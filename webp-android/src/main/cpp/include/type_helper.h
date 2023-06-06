//
// Created by udara on 6/4/23.
//

#pragma once

#include <jni.h>

int getIntegerValue(JNIEnv *env, jobject *object);

bool getBooleanValue(JNIEnv *env, jobject *object);

long getLongValue(JNIEnv *env, jobject *object);

float getFloatValue(JNIEnv *env, jobject *object);

jobject getObjectField(
        JNIEnv *env,
        jobject *object,
        const char *name,
        const char *sig
);