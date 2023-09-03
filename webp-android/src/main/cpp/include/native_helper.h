//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>

class JavaClass {
public:
    inline static jclass booleanClass = nullptr;
    inline static jclass integerClass = nullptr;
    inline static jclass floatClass = nullptr;
    inline static jclass longClass = nullptr;
    inline static jclass encoderClass = nullptr;
    inline static jclass animEncoderClass = nullptr;
    inline static jclass webPConfigClass = nullptr;
    inline static jclass decoderClass = nullptr;
    inline static jclass decoderConfigClass = nullptr;
    inline static jclass webPInfoClass = nullptr;
    inline static jclass bitmapClass = nullptr;
    inline static jclass bitmapConfigClass = nullptr;
    inline static jclass bitmapUtilsClass = nullptr;
    inline static jclass bitmapCompressFormatClass = nullptr;
    inline static jclass webPPresetClass = nullptr;
    inline static jclass throwableClass = nullptr;
    inline static jclass nullPointerExceptionClass = nullptr;
    inline static jclass illegalArgumentExceptionClass = nullptr;
    inline static jclass arrayIndexOutOfBoundsExceptionClass = nullptr;
    inline static jclass ioExceptionClass = nullptr;
    inline static jclass fileNotFoundExceptionClass = nullptr;
    inline static jclass arithmeticExceptionClass = nullptr;
    inline static jclass unsupportedOperationExceptionClass = nullptr;
    inline static jclass illegalStateExceptionClass = nullptr;
    inline static jclass noSuchElementExceptionClass = nullptr;
    inline static jclass numberFormatExceptionClass = nullptr;
    inline static jclass runtimeExceptionClass = nullptr;
    inline static jclass cancellationExceptionClass = nullptr;
    inline static jclass parcelFileDescriptorClass = nullptr;
    inline static jclass uriClass = nullptr;
    inline static jclass uriExtensionsClass = nullptr;
    inline static jclass contextClass = nullptr;
    inline static jclass contentResolverClass = nullptr;

    static void initialize(JNIEnv *env) {
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
        webPConfigClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPConfig")));
        decoderConfigClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("com/aureusapps/android/webpandroid/decoder/DecoderConfig")));
        webPPresetClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPPreset")));
        webPInfoClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPInfo")));
        bitmapClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/graphics/Bitmap")));
        bitmapConfigClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/graphics/Bitmap$Config")));
        bitmapUtilsClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("com/aureusapps/android/extensions/BitmapUtils")));
        bitmapCompressFormatClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/graphics/Bitmap$CompressFormat")));
        throwableClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/Throwable")));
        nullPointerExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/NullPointerException")));
        illegalArgumentExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/IllegalArgumentException")));
        arrayIndexOutOfBoundsExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException")));
        ioExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/io/IOException")));
        fileNotFoundExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/io/FileNotFoundException")));
        arithmeticExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/ArithmeticException")));
        unsupportedOperationExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/UnsupportedOperationException")));
        illegalStateExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/IllegalStateException")));
        noSuchElementExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/util/NoSuchElementException")));
        numberFormatExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/NumberFormatException")));
        runtimeExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/lang/RuntimeException")));
        cancellationExceptionClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("java/util/concurrent/CancellationException")));
        parcelFileDescriptorClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/os/ParcelFileDescriptor")));
        uriClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/net/Uri")));
        uriExtensionsClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("com/aureusapps/android/extensions/UriExtensionsKt")));
        contextClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/content/Context")));
        contentResolverClass = static_cast<jclass>(env->NewGlobalRef(
                env->FindClass("android/content/ContentResolver")));
    }

    static void release(JNIEnv *env) {
        env->DeleteGlobalRef(booleanClass);
        env->DeleteGlobalRef(integerClass);
        env->DeleteGlobalRef(floatClass);
        env->DeleteGlobalRef(longClass);
        env->DeleteGlobalRef(encoderClass);
        env->DeleteGlobalRef(animEncoderClass);
        env->DeleteGlobalRef(webPConfigClass);
        env->DeleteGlobalRef(decoderClass);
        env->DeleteGlobalRef(decoderConfigClass);
        env->DeleteGlobalRef(webPInfoClass);
        env->DeleteGlobalRef(bitmapClass);
        env->DeleteGlobalRef(bitmapConfigClass);
        env->DeleteGlobalRef(bitmapUtilsClass);
        env->DeleteGlobalRef(bitmapCompressFormatClass);
        env->DeleteGlobalRef(webPPresetClass);
        env->DeleteGlobalRef(throwableClass);
        env->DeleteGlobalRef(nullPointerExceptionClass);
        env->DeleteGlobalRef(illegalArgumentExceptionClass);
        env->DeleteGlobalRef(arrayIndexOutOfBoundsExceptionClass);
        env->DeleteGlobalRef(ioExceptionClass);
        env->DeleteGlobalRef(fileNotFoundExceptionClass);
        env->DeleteGlobalRef(arithmeticExceptionClass);
        env->DeleteGlobalRef(unsupportedOperationExceptionClass);
        env->DeleteGlobalRef(illegalStateExceptionClass);
        env->DeleteGlobalRef(noSuchElementExceptionClass);
        env->DeleteGlobalRef(numberFormatExceptionClass);
        env->DeleteGlobalRef(runtimeExceptionClass);
        env->DeleteGlobalRef(cancellationExceptionClass);
        env->DeleteGlobalRef(parcelFileDescriptorClass);
        env->DeleteGlobalRef(uriClass);
        env->DeleteGlobalRef(uriExtensionsClass);
        env->DeleteGlobalRef(contextClass);
        env->DeleteGlobalRef(contentResolverClass);
    }
};