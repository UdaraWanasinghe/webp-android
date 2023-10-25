//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>

#include <functional>

template<typename T>
class Lazy {
protected:
    std::function<T(JNIEnv *)> initializer;
    T value;

public:
    Lazy(std::function<T(JNIEnv *)> init) : initializer(init), value(nullptr) {}

    T get(JNIEnv *env) {
        if (value == nullptr) {
            value = initializer(env);
        }
        return value;
    }

    virtual void reset(JNIEnv *env) {
        value = nullptr;
    }
};

class ClassRegistry {
public:

    static Lazy<jclass> booleanClass;
    static Lazy<jclass> integerClass;
    static Lazy<jclass> floatClass;
    static Lazy<jclass> webPEncoderClass;
    static Lazy<jclass> webPAnimEncoderClass;
    static Lazy<jclass> webPConfigClass;
    static Lazy<jclass> webPDecoderClass;
    static Lazy<jclass> webPDecoderConfigClass;
    static Lazy<jclass> webPInfoClass;
    static Lazy<jclass> bitmapClass;
    static Lazy<jclass> bitmapConfigClass;
    static Lazy<jclass> bitmapUtilsClass;
    static Lazy<jclass> bitmapCompressFormatClass;
    static Lazy<jclass> webPPresetClass;
    static Lazy<jclass> runtimeExceptionClass;
    static Lazy<jclass> cancellationExceptionClass;
    static Lazy<jclass> parcelFileDescriptorClass;
    static Lazy<jclass> uriClass;
    static Lazy<jclass> uriExtensionsClass;
    static Lazy<jclass> contextClass;
    static Lazy<jclass> contentResolverClass;
    static Lazy<jclass> webPAnimEncoderOptionsClass;
    static Lazy<jclass> webPMuxAnimParamsClass;

    static Lazy<jfieldID> encoderPointerFieldID;
    static Lazy<jfieldID> webPAnimEncoderPointerFieldID;
    static Lazy<jfieldID> webPDecoderPointerFieldID;
    static Lazy<jfieldID> uriEmptyFieldID;
    static Lazy<jfieldID> bitmapConfigARGB8888FieldID;
    static Lazy<jfieldID> compressFormatJPEGFieldID;
    static Lazy<jfieldID> compressFormatPNGFieldID;
    static Lazy<jfieldID> compressFormatWEBPFieldID;
    static Lazy<jfieldID> compressFormatWEBPLossyFieldID;
    static Lazy<jfieldID> compressFormatWEBPLosslessFieldID;
    static Lazy<jfieldID> decoderConfigNamePrefixFieldID;
    static Lazy<jfieldID> decoderConfigRepeatCharacterFieldID;
    static Lazy<jfieldID> decoderConfigRepeatCharacterCountFieldID;
    static Lazy<jfieldID> decoderConfigCompressFormatFieldID;
    static Lazy<jfieldID> decoderConfigCompressQualityFieldID;
    static Lazy<jfieldID> webPConfigLosslessFieldID;
    static Lazy<jfieldID> webPConfigQualityFieldID;
    static Lazy<jfieldID> webPConfigMethodFieldID;
    static Lazy<jfieldID> webPConfigTargetSizeFieldID;
    static Lazy<jfieldID> webPConfigTargetPSNRFieldID;
    static Lazy<jfieldID> webPConfigSegmentsFieldID;
    static Lazy<jfieldID> webPConfigSnsStrengthFieldID;
    static Lazy<jfieldID> webPConfigFilterStrengthFieldID;
    static Lazy<jfieldID> webPConfigFilterSharpnessFieldID;
    static Lazy<jfieldID> webPConfigFilterTypeFieldID;
    static Lazy<jfieldID> webPConfigAutoFilterFieldID;
    static Lazy<jfieldID> webPConfigAlphaCompressionFieldID;
    static Lazy<jfieldID> webPConfigAlphaFilteringFieldID;
    static Lazy<jfieldID> webPConfigAlphaQualityFieldID;
    static Lazy<jfieldID> webPConfigPassFieldID;
    static Lazy<jfieldID> webPConfigShowCompressedFieldID;
    static Lazy<jfieldID> webPConfigPreprocessingFieldID;
    static Lazy<jfieldID> webPConfigPartitionsFieldID;
    static Lazy<jfieldID> webPConfigPartitionLimitFieldID;
    static Lazy<jfieldID> webPConfigEmulateJPEGSizeFieldID;
    static Lazy<jfieldID> webPConfigThreadLevelFieldID;
    static Lazy<jfieldID> webPConfigLowMemoryFieldID;
    static Lazy<jfieldID> webPConfigNearLosslessFieldID;
    static Lazy<jfieldID> webPConfigExactFieldID;
    static Lazy<jfieldID> webPConfigUseDeltaPaletteFieldID;
    static Lazy<jfieldID> webPConfigUseSharpYUVFieldID;
    static Lazy<jfieldID> webPConfigQMinFieldID;
    static Lazy<jfieldID> webPConfigQMaxFieldID;
    static Lazy<jfieldID> webPAnimEncoderOptionsMinimizeSizeFieldID;
    static Lazy<jfieldID> webPAnimEncoderOptionsKMinFieldID;
    static Lazy<jfieldID> webPAnimEncoderOptionsKMaxFieldID;
    static Lazy<jfieldID> webPAnimEncoderOptionsAllowMixedFieldID;
    static Lazy<jfieldID> webPAnimEncoderOptionsVerboseFieldID;
    static Lazy<jfieldID> webPAnimEncoderOptionsAnimParamsFieldID;
    static Lazy<jfieldID> webPMuxAnimParamsBgColorFieldID;
    static Lazy<jfieldID> webPMuxAnimParamsLoopCountFieldID;
    static Lazy<jfieldID> webPPresetOrdinalFieldID;

    static Lazy<jmethodID> encoderNotifyProgressMethodID;
    static Lazy<jmethodID> animEncoderNotifyProgressMethodID;
    static Lazy<jmethodID> webPInfoConstructorID;
    static Lazy<jmethodID> decoderNotifyInfoDecodedMethodID;
    static Lazy<jmethodID> decoderNotifyFrameDecodedMethodID;
    static Lazy<jmethodID> bitmapCreateMethodID;
    static Lazy<jmethodID> bitmapCreateScaledMethodID;
    static Lazy<jmethodID> bitmapUtilsSaveInDirectoryMethodID;
    static Lazy<jmethodID> bitmapRecycleMethodID;
    static Lazy<jmethodID> contextGetContentResolverMethodID;
    static Lazy<jmethodID> contentResolverOpenFileDescriptorMethodID;
    static Lazy<jmethodID> parcelFileDescriptorGetFdMethodID;
    static Lazy<jmethodID> parcelFileDescriptorCloseMethodID;
    static Lazy<jmethodID> parcelFileDescriptorCloseWithErrorMethodID;
    static Lazy<jmethodID> uriExtensionsReadToBufferMethodID;
    static Lazy<jmethodID> uriExtensionsFindFileMethodID;
    static Lazy<jmethodID> booleanValueMethodID;
    static Lazy<jmethodID> integerValueMethodID;
    static Lazy<jmethodID> floatValueMethodID;
    static Lazy<jmethodID> bitmapCompressFormatOrdinalMethodID;

    static void release(JNIEnv *env);
};