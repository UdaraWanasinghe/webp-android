//
// Created by udara on 11/2/21.
//

#pragma once

#include <jni.h>

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_create(
        JNIEnv *env,
        jobject self,
        jint width,
        jint height,
        jobject options
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure__Lcom_aureusapps_android_webpandroid_encoder_WebPConfig_2(
        JNIEnv *env,
        jobject self,
        jobject config
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure__Lcom_aureusapps_android_webpandroid_encoder_WebPPreset_2F(
        JNIEnv *env,
        jobject encoder,
        jobject preset,
        jfloat quality
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_addFrame(
        JNIEnv *env,
        jobject encoder,
        jobject frame
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_assemble(
        JNIEnv *env,
        jobject encoder,
        jlong timestamp,
        jstring path
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_release(
        JNIEnv *env,
        jobject encoder
);