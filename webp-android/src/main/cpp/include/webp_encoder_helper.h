//
// Created by udara on 11/3/21.
//

#pragma once

#include <jni.h>
#include <stdlib.h>
#include <string>
#include <webp/encode.h>
#include <webp/mux.h>

WebPPreset parseWebPPreset(JNIEnv *env, jobject *object);

void parseWebPConfig(JNIEnv *env, jobject *object, WebPConfig *config);

float parseWebPQuality(JNIEnv *env, jobject *config);

void parseAnimEncoderOptions(JNIEnv *env, jobject *object, WebPAnimEncoderOptions *options);