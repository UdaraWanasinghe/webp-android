//
// Created by udara on 11/5/21.
//

#ifndef WEBP_ANDROID_DECODER_H
#define WEBP_ANDROID_DECODER_H

#endif //WEBP_ANDROID_DECODER_H

#include <jni.h>

extern "C" jlong
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_create(JNIEnv *, jobject, jobject);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_start(JNIEnv
*, jobject);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_stop(JNIEnv
*, jobject);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_release(JNIEnv
*, jobject);