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
    inline static jfieldID decoderConfigNamePrefixFieldID = nullptr;
    inline static jfieldID decoderConfigRepeatCharacterFieldID = nullptr;
    inline static jfieldID decoderConfigRepeatCharacterCountFieldID = nullptr;
    inline static jfieldID decoderConfigCompressFormatFieldID = nullptr;
    inline static jfieldID decoderConfigCompressQualityFieldID = nullptr;

    inline static jmethodID encoderNotifyProgressMethodID = nullptr;
    inline static jmethodID animEncoderNotifyProgressMethodID = nullptr;
    inline static jmethodID webPInfoConstructorID = nullptr;
    inline static jmethodID decoderNotifyInfoDecodedMethodID = nullptr;
    inline static jmethodID decoderNotifyFrameDecodedMethodID = nullptr;
    inline static jmethodID bitmapCreateMethodID = nullptr;
    inline static jmethodID bitmapCreateScaledMethodID = nullptr;
    inline static jmethodID bitmapUtilsSaveInDirectoryMethodID = nullptr;
    inline static jmethodID bitmapRecycleMethodID = nullptr;
    inline static jmethodID contextGetContentResolverMethodID = nullptr;
    inline static jmethodID contentResolverOpenFileDescriptorMethodID = nullptr;
    inline static jmethodID parcelFileDescriptorGetFdMethodID = nullptr;
    inline static jmethodID parcelFileDescriptorCloseMethodID = nullptr;
    inline static jmethodID parcelFileDescriptorCloseWithErrorMethodID = nullptr;
    inline static jmethodID uriExtensionsReadToBufferMethodID = nullptr;
    inline static jmethodID uriExtensionsFileExistsMethodID = nullptr;
    inline static jmethodID booleanValueMethodID = nullptr;
    inline static jmethodID integerValueMethodID = nullptr;
    inline static jmethodID floatValueMethodID = nullptr;
    inline static jmethodID bitmapCompressFormatOrdinalMethodID = nullptr;

    static void initialize(JNIEnv *env);

    static void release(JNIEnv *env);
};