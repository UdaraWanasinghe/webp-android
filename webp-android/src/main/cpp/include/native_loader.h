//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>

class ClassRegistry {
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

    inline static jfieldID encoderPointerFieldID = nullptr;
    inline static jfieldID animEncoderPointerFieldID = nullptr;
    inline static jfieldID decoderPointerFieldID = nullptr;
    inline static jfieldID uriEmptyFieldID = nullptr;
    inline static jfieldID bitmapConfigARGB8888FieldID = nullptr;
    inline static jfieldID compressFormatJPEGFieldID = nullptr;
    inline static jfieldID compressFormatPNGFieldID = nullptr;
    inline static jfieldID compressFormatWEBPFieldID = nullptr;
    inline static jfieldID compressFormatWEBPLossyFieldID = nullptr;
    inline static jfieldID compressFormatWEBPLosslessFieldID = nullptr;

    inline static jmethodID encoderNotifyProgressMethodID = nullptr;
    inline static jmethodID animEncoderNotifyProgressMethodID = nullptr;
    inline static jmethodID webPInfoConstructorID = nullptr;
    inline static jmethodID decoderNotifyInfoDecodedMethodID = nullptr;
    inline static jmethodID decoderNotifyFrameDecodedMethodID = nullptr;
    inline static jmethodID bitmapCreateMethodID = nullptr;
    inline static jmethodID bitmapUtilsDecodeUriMethodId = nullptr;
    inline static jmethodID bitmapCreateScaledMethodID = nullptr;
    inline static jmethodID bitmapUtilsSaveInDirectoryMethodID = nullptr;
    inline static jmethodID bitmapRecycleMethodID = nullptr;

    static void initialize(JNIEnv *env);

    static void release(JNIEnv *env);
};