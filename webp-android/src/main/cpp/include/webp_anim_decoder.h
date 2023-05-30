//
// Created by udara on 11/5/21.
//

#pragma once

#include <jni.h>

extern "C" jlong
Java_com_aureusapps_android_webpandroid_decoder_WebPAnimDecoder_create(
        JNIEnv *env,
        jobject decoder,
        jobject options
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPAnimDecoder_start(
        JNIEnv *env,
        jobject decoder
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPAnimDecoder_stop(
        JNIEnv *env,
        jobject decoder
);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPAnimDecoder_release(
        JNIEnv *env,
        jobject decoder
);