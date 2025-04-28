//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>

#include <functional>
#include <utility>

template<typename T>
class Lazy {
protected:
    std::function<T(JNIEnv *)> initializer;
    T value;

public:
    explicit Lazy(std::function<T(JNIEnv *)> init) : initializer(std::move(init)), value(nullptr) {}

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

class LazyClass : public Lazy<jclass> {
public:
    explicit LazyClass(const char *name) : Lazy([name](JNIEnv *env) {
        jclass clazz = env->FindClass(name);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return reinterpret_cast<jclass>(env->NewGlobalRef(clazz));
    }) {}

    void reset(JNIEnv *env) override {
        env->DeleteGlobalRef(value);
        value = nullptr;
    }
};

class LazyField : public Lazy<jfieldID> {
public:
    LazyField(Lazy<jclass> &clazz, const char *name, const char *sig) : Lazy(
            [&clazz, name, sig](JNIEnv *env) {
                jclass c = clazz.get(env);
                jfieldID fieldID = env->GetFieldID(c, name, sig);
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                }
                return fieldID;
            }) {}
};

class LazyStaticField : public Lazy<jfieldID> {
public:
    LazyStaticField(Lazy<jclass> &clazz, const char *name, const char *sig) : Lazy(
            [&clazz, name, sig](JNIEnv *env) {
                jclass c = clazz.get(env);
                jfieldID fieldID = env->GetStaticFieldID(c, name, sig);
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                }
                return fieldID;
            }) {}
};

class LazyMethod : public Lazy<jmethodID> {
public:
    LazyMethod(Lazy<jclass> &clazz, const char *name, const char *sig) : Lazy(
            [&clazz, name, sig](JNIEnv *env) {
                jclass c = clazz.get(env);
                jmethodID methodID = env->GetMethodID(c, name, sig);
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                }
                return methodID;
            }) {}
};

class LazyStaticMethod : public Lazy<jmethodID> {
public:
    LazyStaticMethod(Lazy<jclass> &clazz, const char *name, const char *sig) : Lazy(
            [&clazz, name, sig](JNIEnv *env) {
                jclass c = clazz.get(env);
                jmethodID methodID = env->GetStaticMethodID(c, name, sig);
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                }
                return methodID;
            }) {}
};

class ClassRegistry {
public:

    static LazyClass bitmapClass;
    static LazyClass bitmapCompressFormatClass;
    static LazyClass bitmapConfigClass;
    static LazyClass bitmapUtilsClass;
    static LazyClass booleanClass;
    static LazyClass cancellationExceptionClass;
    static LazyClass contentResolverClass;
    static LazyClass contextClass;
    static LazyClass floatClass;
    static LazyClass frameDecodeResultClass;
    static LazyClass infoDecodeResultClass;
    static LazyClass integerClass;
    static LazyClass parcelFileDescriptorClass;
    static LazyClass runtimeExceptionClass;
    static LazyClass uriClass;
    static LazyClass uriExtensionsClass;
    static LazyClass webPAnimEncoderClass;
    static LazyClass webPAnimEncoderOptionsClass;
    static LazyClass webPConfigClass;
    static LazyClass webPDecoderClass;
    static LazyClass webPDecoderConfigClass;
    static LazyClass webPEncoderClass;
    static LazyClass webPInfoClass;
    static LazyClass webPMuxAnimParamsClass;
    static LazyClass webPPresetClass;

    static LazyField decoderConfigCompressFormatFieldID;
    static LazyField decoderConfigCompressQualityFieldID;
    static LazyField decoderConfigNamePrefixFieldID;
    static LazyField decoderConfigRepeatCharacterCountFieldID;
    static LazyField decoderConfigRepeatCharacterFieldID;
    static LazyField encoderPointerFieldID;
    static LazyField webPAnimEncoderOptionsAllowMixedFieldID;
    static LazyField webPAnimEncoderOptionsAnimParamsFieldID;
    static LazyField webPAnimEncoderOptionsKMaxFieldID;
    static LazyField webPAnimEncoderOptionsKMinFieldID;
    static LazyField webPAnimEncoderOptionsMinimizeSizeFieldID;
    static LazyField webPAnimEncoderOptionsVerboseFieldID;
    static LazyField webPAnimEncoderPointerFieldID;
    static LazyField webPConfigAlphaCompressionFieldID;
    static LazyField webPConfigAlphaFilteringFieldID;
    static LazyField webPConfigAlphaQualityFieldID;
    static LazyField webPConfigAutoFilterFieldID;
    static LazyField webPConfigEmulateJPEGSizeFieldID;
    static LazyField webPConfigExactFieldID;
    static LazyField webPConfigFilterSharpnessFieldID;
    static LazyField webPConfigFilterStrengthFieldID;
    static LazyField webPConfigFilterTypeFieldID;
    static LazyField webPConfigLosslessFieldID;
    static LazyField webPConfigLowMemoryFieldID;
    static LazyField webPConfigMethodFieldID;
    static LazyField webPConfigNearLosslessFieldID;
    static LazyField webPConfigPartitionLimitFieldID;
    static LazyField webPConfigPartitionsFieldID;
    static LazyField webPConfigPassFieldID;
    static LazyField webPConfigPreprocessingFieldID;
    static LazyField webPConfigQMaxFieldID;
    static LazyField webPConfigQMinFieldID;
    static LazyField webPConfigQualityFieldID;
    static LazyField webPConfigSegmentsFieldID;
    static LazyField webPConfigShowCompressedFieldID;
    static LazyField webPConfigSnsStrengthFieldID;
    static LazyField webPConfigTargetPSNRFieldID;
    static LazyField webPConfigTargetSizeFieldID;
    static LazyField webPConfigThreadLevelFieldID;
    static LazyField webPConfigUseDeltaPaletteFieldID;
    static LazyField webPConfigUseSharpYUVFieldID;
    static LazyField webPDecoderPointerFieldID;
    static LazyField webPMuxAnimParamsBgColorFieldID;
    static LazyField webPMuxAnimParamsLoopCountFieldID;
    static LazyField webPPresetOrdinalFieldID;

    static LazyStaticField bitmapConfigARGB8888FieldID;
    static LazyStaticField compressFormatJPEGFieldID;
    static LazyStaticField compressFormatPNGFieldID;
    static LazyStaticField compressFormatWEBPFieldID;
    static LazyStaticField compressFormatWEBPLosslessFieldID;
    static LazyStaticField compressFormatWEBPLossyFieldID;
    static LazyStaticField uriEmptyFieldID;

    static LazyMethod animEncoderNotifyProgressMethodID;
    static LazyMethod bitmapCompressFormatOrdinalMethodID;
    static LazyMethod bitmapRecycleMethodID;
    static LazyMethod booleanValueMethodID;
    static LazyMethod contentResolverOpenFileDescriptorMethodID;
    static LazyMethod contextGetContentResolverMethodID;
    static LazyMethod decoderNotifyFrameDecodedMethodID;
    static LazyMethod decoderNotifyInfoDecodedMethodID;
    static LazyMethod encoderNotifyProgressMethodID;
    static LazyMethod floatValueMethodID;
    static LazyMethod frameDecodeResultConstructorID;
    static LazyMethod infoDecodeResultConstructorID;
    static LazyMethod integerValueMethodID;
    static LazyMethod parcelFileDescriptorCloseMethodID;
    static LazyMethod parcelFileDescriptorCloseWithErrorMethodID;
    static LazyMethod parcelFileDescriptorGetFdMethodID;
    static LazyMethod webPInfoConstructorID;

    static LazyStaticMethod bitmapCreateMethodID;
    static LazyStaticMethod bitmapUtilsSaveInDirectoryMethodID;
    static LazyStaticMethod uriExtensionsFindFileMethodID;
    static LazyStaticMethod uriExtensionsReadToBufferMethodID;

    static void release(JNIEnv *env);
};