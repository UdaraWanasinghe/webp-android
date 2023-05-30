//
// Created by udara on 5/29/23.
//

#pragma once

#include <jni.h>
#include <webp/decode.h>

extern "C"
JNIEXPORT jobject JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPInfoDecoder_getWebPFeatures(
        JNIEnv *env,
        jobject thiz,
        jstring path
);