//
// Created by udara on 11/2/21.
//

#ifndef WEBP_ANDROID_ENCODER_H
#define WEBP_ANDROID_ENCODER_H

#endif //WEBP_ANDROID_ENCODER_H

#include <jni.h>

extern "C" JNIEXPORT jlong

JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_create(JNIEnv *, jobject,
                                                                       jint width, jint height,
                                                                       jobject options);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure__Lcom_aureusapps_android_webpandroid_encoder_WebPConfig_2(
        JNIEnv
        *, jobject,
        jobject);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure__Lcom_aureusapps_android_webpandroid_encoder_WebPPreset_2F(
        JNIEnv
        *,
        jobject,
        jobject, jfloat);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_addFrame(JNIEnv
                                                                         *, jobject, jobject);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_assemble(JNIEnv
                                                                         *, jobject, jlong,
                                                                         jstring);

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_release(JNIEnv
                                                                        *, jobject);