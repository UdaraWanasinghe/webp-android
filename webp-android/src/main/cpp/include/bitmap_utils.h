//
// Created by udara on 6/8/23.
//
#pragma once

#include <jni.h>

/**
 * Decode bitmap from the given android uri.
 *
 * @param env Pointer to Java environment.
 * @param jcontext Pointer to android context.
 * @param juri Pointer to android bitmap Uri to decode.
 *
 * @return Android bitmap or null if failed to decode.
 */
jobject decodeBitmapUri(JNIEnv *env, jobject *jcontext, jobject *juri);

jobject resizeBitmap(JNIEnv *env, jobject *jbitmap, int width, int height);

void recycleBitmap(JNIEnv *env, jobject *bitmap);