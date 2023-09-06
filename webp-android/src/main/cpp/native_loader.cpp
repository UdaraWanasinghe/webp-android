//
// Created by udara on 9/3/23.
//

#include "include/native_loader.h"
#include "include/webp_encoder.h"
#include "include/webp_anim_encoder.h"
#include "include/webp_decoder.h"

class LazyClass : public Lazy<jclass> {
public:
    LazyClass(const char *name) : Lazy([name](JNIEnv *env) {
        jclass clazz = env->FindClass(name);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return static_cast<jclass>(env->NewGlobalRef(clazz));
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

Lazy<jclass> ClassRegistry::booleanClass = LazyClass("java/lang/Boolean");
Lazy<jclass> ClassRegistry::integerClass = LazyClass("java/lang/Integer");
Lazy<jclass> ClassRegistry::floatClass = LazyClass("java/lang/Float");
Lazy<jclass> ClassRegistry::webPEncoderClass = LazyClass(
        "com/aureusapps/android/webpandroid/encoder/WebPEncoder");
Lazy<jclass> ClassRegistry::webPAnimEncoderClass = LazyClass(
        "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder");
Lazy<jclass> ClassRegistry::webPDecoderClass = LazyClass(
        "com/aureusapps/android/webpandroid/decoder/WebPDecoder");
Lazy<jclass> ClassRegistry::webPConfigClass = LazyClass(
        "com/aureusapps/android/webpandroid/encoder/WebPConfig");
Lazy<jclass> ClassRegistry::webPDecoderConfigClass = LazyClass(
        "com/aureusapps/android/webpandroid/decoder/DecoderConfig");
Lazy<jclass> ClassRegistry::webPPresetClass = LazyClass(
        "com/aureusapps/android/webpandroid/encoder/WebPPreset");
Lazy<jclass> ClassRegistry::webPInfoClass = LazyClass(
        "com/aureusapps/android/webpandroid/decoder/WebPInfo");
Lazy<jclass> ClassRegistry::bitmapClass = LazyClass("android/graphics/Bitmap");
Lazy<jclass> ClassRegistry::bitmapConfigClass = LazyClass("android/graphics/Bitmap$Config");
Lazy<jclass> ClassRegistry::bitmapUtilsClass = LazyClass(
        "com/aureusapps/android/extensions/BitmapUtils");
Lazy<jclass> ClassRegistry::bitmapCompressFormatClass = LazyClass(
        "android/graphics/Bitmap$CompressFormat");
Lazy<jclass> ClassRegistry::runtimeExceptionClass = LazyClass("java/lang/RuntimeException");
Lazy<jclass> ClassRegistry::cancellationExceptionClass = LazyClass(
        "java/util/concurrent/CancellationException");
Lazy<jclass> ClassRegistry::parcelFileDescriptorClass = LazyClass(
        "android/os/ParcelFileDescriptor");
Lazy<jclass> ClassRegistry::uriClass = LazyClass("android/net/Uri");
Lazy<jclass> ClassRegistry::uriExtensionsClass = LazyClass(
        "com/aureusapps/android/extensions/UriExtensionsKt");
Lazy<jclass> ClassRegistry::contextClass = LazyClass("android/content/Context");
Lazy<jclass> ClassRegistry::contentResolverClass = LazyClass("android/content/ContentResolver");
Lazy<jclass> ClassRegistry::webPAnimEncoderOptionsClass = LazyClass(
        "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoderOptions");
Lazy<jclass> ClassRegistry::webPMuxAnimParamsClass = LazyClass(
        "com/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams");

Lazy<jfieldID> ClassRegistry::encoderPointerFieldID = LazyField(
        webPEncoderClass,
        "nativePointer",
        "J"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderPointerFieldID = LazyField(
        webPAnimEncoderClass,
        "nativePointer",
        "J"
);
Lazy<jfieldID> ClassRegistry::webPDecoderPointerFieldID = LazyField(
        webPDecoderClass,
        "nativePointer",
        "J"
);
Lazy<jfieldID> ClassRegistry::uriEmptyFieldID = LazyStaticField(
        uriClass,
        "EMPTY",
        "Landroid/net/Uri;"
);
Lazy<jfieldID> ClassRegistry::bitmapConfigARGB8888FieldID = LazyStaticField(
        bitmapConfigClass,
        "ARGB_8888",
        "Landroid/graphics/Bitmap$Config;"
);
Lazy<jfieldID> ClassRegistry::compressFormatJPEGFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "JPEG",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
Lazy<jfieldID> ClassRegistry::compressFormatPNGFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "PNG",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
Lazy<jfieldID> ClassRegistry::compressFormatWEBPFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "WEBP",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
Lazy<jfieldID> ClassRegistry::compressFormatWEBPLossyFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "WEBP_LOSSY",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
Lazy<jfieldID> ClassRegistry::compressFormatWEBPLosslessFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "WEBP_LOSSLESS",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
Lazy<jfieldID> ClassRegistry::decoderConfigNamePrefixFieldID = LazyField(
        webPDecoderConfigClass,
        "namePrefix",
        "Ljava/lang/String;"
);
Lazy<jfieldID> ClassRegistry::decoderConfigRepeatCharacterFieldID = LazyField(
        webPDecoderConfigClass,
        "repeatCharacter",
        "C"
);
Lazy<jfieldID> ClassRegistry::decoderConfigRepeatCharacterCountFieldID = LazyField(
        webPDecoderConfigClass,
        "repeatCharacterCount",
        "I"
);
Lazy<jfieldID> ClassRegistry::decoderConfigCompressFormatFieldID = LazyField(
        webPDecoderConfigClass,
        "compressFormat",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
Lazy<jfieldID> ClassRegistry::decoderConfigCompressQualityFieldID = LazyField(
        webPDecoderConfigClass,
        "compressQuality",
        "I"
);
Lazy<jfieldID> ClassRegistry::webPConfigLosslessFieldID = LazyField(
        webPConfigClass,
        "lossless",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigQualityFieldID = LazyField(
        webPConfigClass,
        "quality",
        "Ljava/lang/Float;"
);
Lazy<jfieldID> ClassRegistry::webPConfigMethodFieldID = LazyField(
        webPConfigClass,
        "method",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigTargetPSNRFieldID = LazyField(
        webPConfigClass,
        "targetPSNR",
        "Ljava/lang/Float;"
);
Lazy<jfieldID> ClassRegistry::webPConfigTargetSizeFieldID = LazyField(
        webPConfigClass,
        "targetSize",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigSegmentsFieldID = LazyField(
        webPConfigClass,
        "segments",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigSnsStrengthFieldID = LazyField(
        webPConfigClass,
        "snsStrength",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigFilterStrengthFieldID = LazyField(
        webPConfigClass,
        "filterStrength",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigFilterSharpnessFieldID = LazyField(
        webPConfigClass,
        "filterSharpness",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigFilterTypeFieldID = LazyField(
        webPConfigClass,
        "filterType",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigAutoFilterFieldID = LazyField(
        webPConfigClass,
        "autoFilter",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigAlphaCompressionFieldID = LazyField(
        webPConfigClass,
        "alphaCompression",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigAlphaFilteringFieldID = LazyField(
        webPConfigClass,
        "alphaFiltering",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigAlphaQualityFieldID = LazyField(
        webPConfigClass,
        "alphaQuality",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigPassFieldID = LazyField(
        webPConfigClass,
        "pass",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigShowCompressedFieldID = LazyField(
        webPConfigClass,
        "showCompressed",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigPreprocessingFieldID = LazyField(
        webPConfigClass,
        "preprocessing",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigPartitionsFieldID = LazyField(
        webPConfigClass,
        "partitions",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigPartitionLimitFieldID = LazyField(
        webPConfigClass,
        "partitionLimit",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigEmulateJPEGSizeFieldID = LazyField(
        webPConfigClass,
        "emulateJPEGSize",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigThreadLevelFieldID = LazyField(
        webPConfigClass,
        "threadLevel",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigLowMemoryFieldID = LazyField(
        webPConfigClass,
        "lowMemory",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigNearLosslessFieldID = LazyField(
        webPConfigClass,
        "nearLossless",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigExactFieldID = LazyField(
        webPConfigClass,
        "exact",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigUseDeltaPaletteFieldID = LazyField(
        webPConfigClass,
        "useDeltaPalette",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigUseSharpYUVFieldID = LazyField(
        webPConfigClass,
        "useSharpYUV",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPConfigQMinFieldID = LazyField(
        webPConfigClass,
        "qmin",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPConfigQMaxFieldID = LazyField(
        webPConfigClass,
        "qmax",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderOptionsMinimizeSizeFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "minimizeSize",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderOptionsKMinFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "kmin",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderOptionsKMaxFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "kmax",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderOptionsAllowMixedFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "allowMixed",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderOptionsVerboseFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "verbose",
        "Ljava/lang/Boolean;"
);
Lazy<jfieldID> ClassRegistry::webPAnimEncoderOptionsAnimParamsFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "animParams",
        "Lcom/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams;"
);
Lazy<jfieldID> ClassRegistry::webPMuxAnimParamsBgColorFieldID = LazyField(
        webPMuxAnimParamsClass,
        "backgroundColor",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPMuxAnimParamsLoopCountFieldID = LazyField(
        webPMuxAnimParamsClass,
        "loopCount",
        "Ljava/lang/Integer;"
);
Lazy<jfieldID> ClassRegistry::webPPresetOrdinalFieldID = LazyField(webPPresetClass, "value", "I");

Lazy<jmethodID> ClassRegistry::booleanValueMethodID = LazyMethod(
        booleanClass,
        "booleanValue",
        "()Z"
);
Lazy<jmethodID> ClassRegistry::integerValueMethodID = LazyMethod(integerClass, "intValue", "()I");
Lazy<jmethodID> ClassRegistry::floatValueMethodID = LazyMethod(floatClass, "floatValue", "()F");
Lazy<jmethodID> ClassRegistry::encoderNotifyProgressMethodID = LazyMethod(
        webPEncoderClass,
        "notifyProgressChanged",
        "(I)Z"
);
Lazy<jmethodID> ClassRegistry::animEncoderNotifyProgressMethodID = LazyMethod(
        webPAnimEncoderClass,
        "notifyProgressChanged",
        "(II)Z"
);
Lazy<jmethodID> ClassRegistry::webPInfoConstructorID = LazyMethod(
        webPInfoClass,
        "<init>",
        "(IIZZIII)V"
);
Lazy<jmethodID> ClassRegistry::decoderNotifyInfoDecodedMethodID = LazyMethod(
        webPDecoderClass,
        "notifyInfoDecoded",
        "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
);
Lazy<jmethodID> ClassRegistry::decoderNotifyFrameDecodedMethodID = LazyMethod(
        webPDecoderClass,
        "notifyFrameDecoded",
        "(IJLandroid/graphics/Bitmap;Landroid/net/Uri;)V"
);
Lazy<jmethodID> ClassRegistry::bitmapCreateMethodID = LazyStaticMethod(
        bitmapClass,
        "createBitmap",
        "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
);
Lazy<jmethodID> ClassRegistry::bitmapCreateScaledMethodID = LazyStaticMethod(
        bitmapClass,
        "createScaledBitmap",
        "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
);
Lazy<jmethodID> ClassRegistry::bitmapUtilsSaveInDirectoryMethodID = LazyStaticMethod(
        bitmapUtilsClass,
        "saveInDirectory",
        "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
);
Lazy<jmethodID> ClassRegistry::bitmapRecycleMethodID = LazyMethod(
        bitmapClass,
        "recycle",
        "()V"
);
Lazy<jmethodID> ClassRegistry::contextGetContentResolverMethodID = LazyMethod(
        contextClass,
        "getContentResolver",
        "()Landroid/content/ContentResolver;"
);
Lazy<jmethodID> ClassRegistry::contentResolverOpenFileDescriptorMethodID = LazyMethod(
        contentResolverClass,
        "openFileDescriptor",
        "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
);
Lazy<jmethodID> ClassRegistry::parcelFileDescriptorGetFdMethodID = LazyMethod(
        parcelFileDescriptorClass,
        "getFd",
        "()I"
);
Lazy<jmethodID> ClassRegistry::parcelFileDescriptorCloseMethodID = LazyMethod(
        parcelFileDescriptorClass,
        "close",
        "()V"
);
Lazy<jmethodID> ClassRegistry::parcelFileDescriptorCloseWithErrorMethodID = LazyMethod(
        parcelFileDescriptorClass,
        "closeWithError",
        "(Ljava/lang/String;)V"
);
Lazy<jmethodID> ClassRegistry::uriExtensionsReadToBufferMethodID = LazyStaticMethod(
        uriExtensionsClass,
        "readToBuffer",
        "(Landroid/net/Uri;Landroid/content/Context;)Ljava/nio/ByteBuffer;"
);
Lazy<jmethodID> ClassRegistry::uriExtensionsFileExistsMethodID = LazyStaticMethod(
        uriExtensionsClass,
        "fileExists",
        "(Landroid/net/Uri;Landroid/content/Context;Ljava/lang/String;)Z"
);
Lazy<jmethodID> ClassRegistry::bitmapCompressFormatOrdinalMethodID = LazyMethod(
        bitmapCompressFormatClass,
        "ordinal",
        "()I"
);

void ClassRegistry::release(JNIEnv *env) {
    booleanClass.reset(env);
    integerClass.reset(env);
    floatClass.reset(env);
    webPEncoderClass.reset(env);
    webPAnimEncoderClass.reset(env);
    webPConfigClass.reset(env);
    webPDecoderClass.reset(env);
    webPDecoderConfigClass.reset(env);
    webPInfoClass.reset(env);
    bitmapClass.reset(env);
    bitmapConfigClass.reset(env);
    bitmapUtilsClass.reset(env);
    bitmapCompressFormatClass.reset(env);
    webPPresetClass.reset(env);
    runtimeExceptionClass.reset(env);
    cancellationExceptionClass.reset(env);
    parcelFileDescriptorClass.reset(env);
    uriClass.reset(env);
    uriExtensionsClass.reset(env);
    contextClass.reset(env);
    contentResolverClass.reset(env);
    webPAnimEncoderOptionsClass.reset(env);
    webPMuxAnimParamsClass.reset(env);
}

static const JNINativeMethod encoderMethods[] = {
        {"nativeCreate",
                "(II)J",
                reinterpret_cast<void *>(WebPEncoder::nativeCreate)
        },
        {"nativeConfigure",
                "(Lcom/aureusapps/android/webpandroid/encoder/WebPConfig;Lcom/aureusapps/android/webpandroid/encoder/WebPPreset;)V",
                reinterpret_cast<void *>(WebPEncoder::nativeConfigure)
        },
        {"nativeEncode",
                "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;)V",
                reinterpret_cast<void *>(WebPEncoder::nativeEncode)
        },
        {"nativeCancel",
                "()V",
                reinterpret_cast<void *>(WebPEncoder::nativeCancel)
        },
        {"nativeRelease",
                "()V",
                reinterpret_cast<void *>(WebPEncoder::nativeRelease)
        },
};

static const JNINativeMethod animEncoderMethods[] = {
        {"nativeCreate",
                "(IILcom/aureusapps/android/webpandroid/encoder/WebPAnimEncoderOptions;)J",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeCreate)
        },
        {"nativeConfigure",
                "(Lcom/aureusapps/android/webpandroid/encoder/WebPConfig;Lcom/aureusapps/android/webpandroid/encoder/WebPPreset;)V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeConfigure)
        },
        {"nativeAddFrame",
                "(JLandroid/graphics/Bitmap;)V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeAddFrame)
        },
        {"nativeAssemble",
                "(Landroid/content/Context;JLandroid/net/Uri;)V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeAssemble)
        },
        {"nativeCancel",
                "()V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeCancel)
        },
        {"nativeRelease",
                "()V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeRelease)
        },
};

static const JNINativeMethod decoderMethods[] = {
        {"nativeCreate",
                "()J",
                reinterpret_cast<void *>(WebPDecoder::nativeCreate)
        },
        {"nativeConfigure",
                "(Lcom/aureusapps/android/webpandroid/decoder/DecoderConfig;)V",
                reinterpret_cast<void *>(WebPDecoder::nativeConfigure)
        },
        {"nativeDecodeFrames",
                "(Landroid/content/Context;Landroid/net/Uri;Landroid/net/Uri;)V",
                reinterpret_cast<void *>(WebPDecoder::nativeDecodeFrames)
        },
        {"nativeDecodeInfo",
                "(Landroid/content/Context;Landroid/net/Uri;)Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;",
                reinterpret_cast<void *>(WebPDecoder::nativeDecodeInfo)
        },
        {"nativeCancel",
                "()V",
                reinterpret_cast<void *>(WebPDecoder::nativeCancel)
        },
        {"nativeRelease",
                "()V",
                reinterpret_cast<void *>(WebPDecoder::nativeRelease)
        }
};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    // register native methods
    // encoder methods
    int result = env->RegisterNatives(
            ClassRegistry::webPEncoderClass.get(env),
            encoderMethods,
            sizeof(encoderMethods) / sizeof(JNINativeMethod)
    );
    if (result != JNI_OK) return result;

    // anim encoder methods
    result = env->RegisterNatives(
            ClassRegistry::webPAnimEncoderClass.get(env),
            animEncoderMethods,
            sizeof(animEncoderMethods) / sizeof(JNINativeMethod)
    );
    if (result != JNI_OK) return result;

    // decoder methods
    result = env->RegisterNatives(
            ClassRegistry::webPDecoderClass.get(env),
            decoderMethods,
            sizeof(decoderMethods) / sizeof(JNINativeMethod)
    );
    if (result != JNI_OK) return result;

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    ClassRegistry::release(env);
}