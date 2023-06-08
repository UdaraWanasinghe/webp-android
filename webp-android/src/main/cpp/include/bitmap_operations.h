//
// Created by udara on 6/8/23.
//
#pragma once

#include <jni.h>

jobject resizeBitmap(JNIEnv *env, jobject *sourceBitmap, int newWidth, int newHeight);

void recycleBitmap(JNIEnv *env, jobject *bitmap);