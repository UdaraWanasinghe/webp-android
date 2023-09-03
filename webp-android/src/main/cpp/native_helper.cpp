//
// Created by udara on 9/3/23.
//

#include <jni.h>
#include "include/native_helper.h"

void JavaClass::initialize(JNIEnv *env) {
    booleanClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/Boolean")));
    integerClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/Integer")));
    floatClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/Float")));
    longClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/Long")));
    encoderClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPEncoder")));
    animEncoderClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder")));
    decoderClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPDecoder")));
    decoderConfigClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/webpandroid/decoder/DecoderConfig")));
    webpInfoClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPInfo")));
    bitmapClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("android/graphics/Bitmap")));
    bitmapConfigClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("android/graphics/Bitmap$Config")));
    bitmapUtilsClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/extensions/BitmapUtils")));
    compressFormatClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("android/graphics/Bitmap$CompressFormat")));
    webpPresetClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPPreset")));
    numberFormatExceptionClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/NumberFormatException")));
    runtimeExceptionClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/RuntimeException")));
    cancellationExceptionClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/util/concurrent/CancellationException")));
    throwableClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("java/lang/Throwable")));
    parcelFileDescriptorClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("android/os/ParcelFileDescriptor")));
    uriClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("android/net/Uri")));
    uriExtensionsClass = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/aureusapps/android/extensions/UriExtensionsKt")));
}

void JavaClass::release(JNIEnv *env) {
    env->DeleteGlobalRef(booleanClass);
    env->DeleteGlobalRef(integerClass);
    env->DeleteGlobalRef(floatClass);
    env->DeleteGlobalRef(longClass);
    env->DeleteGlobalRef(encoderClass);
    env->DeleteGlobalRef(animEncoderClass);
    env->DeleteGlobalRef(decoderClass);
    env->DeleteGlobalRef(decoderConfigClass);
    env->DeleteGlobalRef(webpInfoClass);
    env->DeleteGlobalRef(bitmapClass);
    env->DeleteGlobalRef(bitmapConfigClass);
    env->DeleteGlobalRef(bitmapUtilsClass);
    env->DeleteGlobalRef(compressFormatClass);
    env->DeleteGlobalRef(webpPresetClass);
    env->DeleteGlobalRef(numberFormatExceptionClass);
    env->DeleteGlobalRef(runtimeExceptionClass);
    env->DeleteGlobalRef(cancellationExceptionClass);
    env->DeleteGlobalRef(throwableClass);
    env->DeleteGlobalRef(parcelFileDescriptorClass);
    env->DeleteGlobalRef(uriClass);
    env->DeleteGlobalRef(uriExtensionsClass);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    JavaClass::initialize(env);
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    JavaClass::release(env);
}