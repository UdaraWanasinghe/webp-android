//
// Created by udara on 11/2/21.
//

#ifndef WEBP_ANDROID_ENCODER_H
#define WEBP_ANDROID_ENCODER_H

#endif //WEBP_ANDROID_ENCODER_H

#include <jni.h>

extern "C" JNIEXPORT jlong JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_create(JNIEnv *, jobject, jint width, jint height, jobject options);

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_config__Lcom_aureusapps_webpcodec_encoder_WebPConfig_2(JNIEnv *, jobject,
                                                                                                             jobject);

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_config__Lcom_aureusapps_webpcodec_encoder_WebPPreset_2F(JNIEnv *, jobject,
                                                                                                              jobject, jfloat);
