//
// Created by udara on 9/3/23.
//

#include "include/native_loader.h"
#include "include/webp_encoder.h"
#include "include/webp_anim_encoder.h"
#include "include/webp_decoder.h"

LazyClass ClassRegistry::bitmapClass = LazyClass("android/graphics/Bitmap");
LazyClass ClassRegistry::bitmapCompressFormatClass = LazyClass("android/graphics/Bitmap$CompressFormat");
LazyClass ClassRegistry::bitmapConfigClass = LazyClass("android/graphics/Bitmap$Config");
LazyClass ClassRegistry::bitmapUtilsClass = LazyClass("com/aureusapps/android/extensions/BitmapUtils");
LazyClass ClassRegistry::booleanClass = LazyClass("java/lang/Boolean");
LazyClass ClassRegistry::cancellationExceptionClass = LazyClass("java/util/concurrent/CancellationException");
LazyClass ClassRegistry::contentResolverClass = LazyClass("android/content/ContentResolver");
LazyClass ClassRegistry::contextClass = LazyClass("android/content/Context");
LazyClass ClassRegistry::floatClass = LazyClass("java/lang/Float");
LazyClass ClassRegistry::frameDecodeResultClass = LazyClass("com/aureusapps/android/webpandroid/decoder/FrameDecodeResult");
LazyClass ClassRegistry::infoDecodeResultClass = LazyClass("com/aureusapps/android/webpandroid/decoder/InfoDecodeResult");
LazyClass ClassRegistry::integerClass = LazyClass("java/lang/Integer");
LazyClass ClassRegistry::parcelFileDescriptorClass = LazyClass("android/os/ParcelFileDescriptor");
LazyClass ClassRegistry::runtimeExceptionClass = LazyClass("java/lang/RuntimeException");
LazyClass ClassRegistry::uriClass = LazyClass("android/net/Uri");
LazyClass ClassRegistry::uriExtensionsClass = LazyClass("com/aureusapps/android/extensions/UriExtensionsKt");
LazyClass ClassRegistry::webPAnimEncoderClass = LazyClass("com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder");
LazyClass ClassRegistry::webPAnimEncoderOptionsClass = LazyClass("com/aureusapps/android/webpandroid/encoder/WebPAnimEncoderOptions");
LazyClass ClassRegistry::webPConfigClass = LazyClass("com/aureusapps/android/webpandroid/encoder/WebPConfig");
LazyClass ClassRegistry::webPDecoderClass = LazyClass("com/aureusapps/android/webpandroid/decoder/WebPDecoder");
LazyClass ClassRegistry::webPDecoderConfigClass = LazyClass("com/aureusapps/android/webpandroid/decoder/DecoderConfig");
LazyClass ClassRegistry::webPEncoderClass = LazyClass("com/aureusapps/android/webpandroid/encoder/WebPEncoder");
LazyClass ClassRegistry::webPInfoClass = LazyClass("com/aureusapps/android/webpandroid/decoder/WebPInfo");
LazyClass ClassRegistry::webPMuxAnimParamsClass = LazyClass("com/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams");
LazyClass ClassRegistry::webPPresetClass = LazyClass("com/aureusapps/android/webpandroid/encoder/WebPPreset");

LazyField ClassRegistry::decoderConfigCompressFormatFieldID = LazyField(
        webPDecoderConfigClass,
        "compressFormat",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
LazyField ClassRegistry::decoderConfigCompressQualityFieldID = LazyField(
        webPDecoderConfigClass,
        "compressQuality",
        "I"
);
LazyField ClassRegistry::decoderConfigNamePrefixFieldID = LazyField(
        webPDecoderConfigClass,
        "namePrefix",
        "Ljava/lang/String;"
);
LazyField ClassRegistry::decoderConfigRepeatCharacterCountFieldID = LazyField(
        webPDecoderConfigClass,
        "repeatCharacterCount",
        "I"
);
LazyField ClassRegistry::decoderConfigRepeatCharacterFieldID = LazyField(
        webPDecoderConfigClass,
        "repeatCharacter",
        "C"
);
LazyField ClassRegistry::encoderPointerFieldID = LazyField(
        webPEncoderClass,
        "nativePointer",
        "J"
);
LazyField ClassRegistry::webPAnimEncoderOptionsAllowMixedFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "allowMixed",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPAnimEncoderOptionsAnimParamsFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "animParams",
        "Lcom/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams;"
);
LazyField ClassRegistry::webPAnimEncoderOptionsKMaxFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "kmax",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPAnimEncoderOptionsKMinFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "kmin",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPAnimEncoderOptionsMinimizeSizeFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "minimizeSize",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPAnimEncoderOptionsVerboseFieldID = LazyField(
        webPAnimEncoderOptionsClass,
        "verbose",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPAnimEncoderPointerFieldID = LazyField(
        webPAnimEncoderClass,
        "nativePointer",
        "J"
);
LazyField ClassRegistry::webPConfigAlphaCompressionFieldID = LazyField(
        webPConfigClass,
        "alphaCompression",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigAlphaFilteringFieldID = LazyField(
        webPConfigClass,
        "alphaFiltering",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigAlphaQualityFieldID = LazyField(
        webPConfigClass,
        "alphaQuality",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigAutoFilterFieldID = LazyField(
        webPConfigClass,
        "autoFilter",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPConfigEmulateJPEGSizeFieldID = LazyField(
        webPConfigClass,
        "emulateJPEGSize",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPConfigExactFieldID = LazyField(
        webPConfigClass,
        "exact",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPConfigFilterSharpnessFieldID = LazyField(
        webPConfigClass,
        "filterSharpness",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigFilterStrengthFieldID = LazyField(
        webPConfigClass,
        "filterStrength",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigFilterTypeFieldID = LazyField(
        webPConfigClass,
        "filterType",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigLosslessFieldID = LazyField(
        webPConfigClass,
        "lossless",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigLowMemoryFieldID = LazyField(
        webPConfigClass,
        "lowMemory",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPConfigMethodFieldID = LazyField(
        webPConfigClass,
        "method",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigNearLosslessFieldID = LazyField(
        webPConfigClass,
        "nearLossless",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigPartitionLimitFieldID = LazyField(
        webPConfigClass,
        "partitionLimit",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigPartitionsFieldID = LazyField(
        webPConfigClass,
        "partitions",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigPassFieldID = LazyField(
        webPConfigClass,
        "pass",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigPreprocessingFieldID = LazyField(
        webPConfigClass,
        "preprocessing",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigQMaxFieldID = LazyField(
        webPConfigClass,
        "qmax",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigQMinFieldID = LazyField(
        webPConfigClass,
        "qmin",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigQualityFieldID = LazyField(
        webPConfigClass,
        "quality",
        "Ljava/lang/Float;"
);
LazyField ClassRegistry::webPConfigSegmentsFieldID = LazyField(
        webPConfigClass,
        "segments",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigShowCompressedFieldID = LazyField(
        webPConfigClass,
        "showCompressed",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPConfigSnsStrengthFieldID = LazyField(
        webPConfigClass,
        "snsStrength",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigTargetPSNRFieldID = LazyField(
        webPConfigClass,
        "targetPSNR",
        "Ljava/lang/Float;"
);
LazyField ClassRegistry::webPConfigTargetSizeFieldID = LazyField(
        webPConfigClass,
        "targetSize",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigThreadLevelFieldID = LazyField(
        webPConfigClass,
        "threadLevel",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPConfigUseDeltaPaletteFieldID = LazyField(
        webPConfigClass,
        "useDeltaPalette",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPConfigUseSharpYUVFieldID = LazyField(
        webPConfigClass,
        "useSharpYUV",
        "Ljava/lang/Boolean;"
);
LazyField ClassRegistry::webPDecoderPointerFieldID = LazyField(
        webPDecoderClass,
        "nativePointer",
        "J"
);
LazyField ClassRegistry::webPMuxAnimParamsBgColorFieldID = LazyField(
        webPMuxAnimParamsClass,
        "backgroundColor",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPMuxAnimParamsLoopCountFieldID = LazyField(
        webPMuxAnimParamsClass,
        "loopCount",
        "Ljava/lang/Integer;"
);
LazyField ClassRegistry::webPPresetOrdinalFieldID = LazyField(
        webPPresetClass,
        "value",
        "I"
);

LazyStaticField ClassRegistry::bitmapConfigARGB8888FieldID = LazyStaticField(
        bitmapConfigClass,
        "ARGB_8888",
        "Landroid/graphics/Bitmap$Config;"
);
LazyStaticField ClassRegistry::compressFormatJPEGFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "JPEG",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
LazyStaticField ClassRegistry::compressFormatPNGFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "PNG",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
LazyStaticField ClassRegistry::compressFormatWEBPFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "WEBP",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
LazyStaticField ClassRegistry::compressFormatWEBPLosslessFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "WEBP_LOSSLESS",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
LazyStaticField ClassRegistry::compressFormatWEBPLossyFieldID = LazyStaticField(
        bitmapCompressFormatClass,
        "WEBP_LOSSY",
        "Landroid/graphics/Bitmap$CompressFormat;"
);
LazyStaticField ClassRegistry::uriEmptyFieldID = LazyStaticField(
        uriClass,
        "EMPTY",
        "Landroid/net/Uri;"
);


LazyMethod ClassRegistry::animEncoderNotifyProgressMethodID = LazyMethod(
        webPAnimEncoderClass,
        "notifyProgressChanged",
        "(II)Z"
);
LazyMethod ClassRegistry::bitmapCompressFormatOrdinalMethodID = LazyMethod(
        bitmapCompressFormatClass,
        "ordinal",
        "()I"
);
LazyMethod ClassRegistry::bitmapRecycleMethodID = LazyMethod(
        bitmapClass,
        "recycle",
        "()V"
);
LazyMethod ClassRegistry::booleanValueMethodID = LazyMethod(
        booleanClass,
        "booleanValue",
        "()Z"
);
LazyMethod ClassRegistry::contentResolverOpenFileDescriptorMethodID = LazyMethod(
        contentResolverClass,
        "openFileDescriptor",
        "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
);
LazyMethod ClassRegistry::contextGetContentResolverMethodID = LazyMethod(
        contextClass,
        "getContentResolver",
        "()Landroid/content/ContentResolver;"
);
LazyMethod ClassRegistry::decoderNotifyFrameDecodedMethodID = LazyMethod(
        webPDecoderClass,
        "notifyFrameDecoded",
        "(IJLandroid/graphics/Bitmap;Landroid/net/Uri;)V"
);
LazyMethod ClassRegistry::decoderNotifyInfoDecodedMethodID = LazyMethod(
        webPDecoderClass,
        "notifyInfoDecoded",
        "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
);
LazyMethod ClassRegistry::encoderNotifyProgressMethodID = LazyMethod(
        webPEncoderClass,
        "notifyProgressChanged",
        "(I)Z"
);
LazyMethod ClassRegistry::floatValueMethodID = LazyMethod(
        floatClass,
        "floatValue",
        "()F"
);
LazyMethod ClassRegistry::frameDecodeResultConstructorID = LazyMethod(
        frameDecodeResultClass,
        "<init>",
        "(Landroid/graphics/Bitmap;II)V"
);
LazyMethod ClassRegistry::infoDecodeResultConstructorID = LazyMethod(
        infoDecodeResultClass,
        "<init>",
        "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;I)V"
);
LazyMethod ClassRegistry::integerValueMethodID = LazyMethod(
        integerClass,
        "intValue",
        "()I"
);
LazyMethod ClassRegistry::parcelFileDescriptorCloseMethodID = LazyMethod(
        parcelFileDescriptorClass,
        "close",
        "()V"
);
LazyMethod ClassRegistry::parcelFileDescriptorCloseWithErrorMethodID = LazyMethod(
        parcelFileDescriptorClass,
        "closeWithError",
        "(Ljava/lang/String;)V"
);
LazyMethod ClassRegistry::parcelFileDescriptorGetFdMethodID = LazyMethod(
        parcelFileDescriptorClass,
        "getFd",
        "()I"
);
LazyMethod ClassRegistry::webPInfoConstructorID = LazyMethod(
        webPInfoClass,
        "<init>",
        "(IIZZIII)V"
);

LazyStaticMethod ClassRegistry::bitmapCreateMethodID = LazyStaticMethod(
        bitmapClass,
        "createBitmap",
        "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
);
LazyStaticMethod ClassRegistry::bitmapCreateScaledMethodID = LazyStaticMethod(
        bitmapClass,
        "createScaledBitmap",
        "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
);
LazyStaticMethod ClassRegistry::bitmapUtilsSaveInDirectoryMethodID = LazyStaticMethod(
        bitmapUtilsClass,
        "saveInDirectory",
        "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
);
LazyStaticMethod ClassRegistry::uriExtensionsFindFileMethodID = LazyStaticMethod(
        uriExtensionsClass,
        "findFile",
        "(Landroid/net/Uri;Landroid/content/Context;Ljava/lang/String;)Landroid/net/Uri;"
);
LazyStaticMethod ClassRegistry::uriExtensionsReadToBufferMethodID = LazyStaticMethod(
        uriExtensionsClass,
        "readToBuffer",
        "(Landroid/net/Uri;Landroid/content/Context;)Ljava/nio/ByteBuffer;"
);

void ClassRegistry::release(JNIEnv *env) {
    bitmapClass.reset(env);
    bitmapCompressFormatClass.reset(env);
    bitmapConfigClass.reset(env);
    bitmapUtilsClass.reset(env);
    booleanClass.reset(env);
    cancellationExceptionClass.reset(env);
    contentResolverClass.reset(env);
    contextClass.reset(env);
    floatClass.reset(env);
    frameDecodeResultClass.reset(env);
    infoDecodeResultClass.reset(env);
    integerClass.reset(env);
    parcelFileDescriptorClass.reset(env);
    runtimeExceptionClass.reset(env);
    uriClass.reset(env);
    uriExtensionsClass.reset(env);
    webPAnimEncoderClass.reset(env);
    webPAnimEncoderOptionsClass.reset(env);
    webPConfigClass.reset(env);
    webPDecoderClass.reset(env);
    webPDecoderConfigClass.reset(env);
    webPEncoderClass.reset(env);
    webPInfoClass.reset(env);
    webPMuxAnimParamsClass.reset(env);
    webPPresetClass.reset(env);
}

static const JNINativeMethod encoderMethods[] = {
        {
                "nativeCreate",
                "(II)J",
                reinterpret_cast<void *>(WebPEncoder::nativeCreate)
        },
        {
                "nativeConfigure",
                "(Lcom/aureusapps/android/webpandroid/encoder/WebPConfig;Lcom/aureusapps/android/webpandroid/encoder/WebPPreset;)V",
                reinterpret_cast<void *>(WebPEncoder::nativeConfigure)
        },
        {
                "nativeEncode",
                "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;)V",
                reinterpret_cast<void *>(WebPEncoder::nativeEncode)
        },
        {
                "nativeCancel",
                "()V",
                reinterpret_cast<void *>(WebPEncoder::nativeCancel)
        },
        {
                "nativeRelease",
                "()V",
                reinterpret_cast<void *>(WebPEncoder::nativeRelease)
        },
};

static const JNINativeMethod animEncoderMethods[] = {
        {
                "nativeCreate",
                "(IILcom/aureusapps/android/webpandroid/encoder/WebPAnimEncoderOptions;)J",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeCreate)
        },
        {
                "nativeConfigure",
                "(Lcom/aureusapps/android/webpandroid/encoder/WebPConfig;Lcom/aureusapps/android/webpandroid/encoder/WebPPreset;)V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeConfigure)
        },
        {
                "nativeAddFrame",
                "(JLandroid/graphics/Bitmap;)V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeAddFrame)
        },
        {
                "nativeAssemble",
                "(Landroid/content/Context;JLandroid/net/Uri;)V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeAssemble)
        },
        {
                "nativeCancel",
                "()V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeCancel)
        },
        {
                "nativeRelease",
                "()V",
                reinterpret_cast<void *>(WebPAnimationEncoder::nativeRelease)
        },
};

static const JNINativeMethod decoderMethods[] = {
        {
                "nativeCreate",
                "()J",
                reinterpret_cast<void *>(WebPDecoder::nativeCreate)
        },
        {
                "nativeConfigure",
                "(Lcom/aureusapps/android/webpandroid/decoder/DecoderConfig;)V",
                reinterpret_cast<void *>(WebPDecoder::nativeConfigure)
        },
        {
                "nativeSetDataSource",
                "(Landroid/content/Context;Landroid/net/Uri;)I",
                reinterpret_cast<void *>(WebPDecoder::nativeSetDataSource)
        },
        {
                "nativeDecodeInfo",
                "()Lcom/aureusapps/android/webpandroid/decoder/InfoDecodeResult;",
                reinterpret_cast<void *>(WebPDecoder::nativeDecodeInfo)
        },
        {
                "nativeDecodeFrames",
                "(Landroid/content/Context;Landroid/net/Uri;)I",
                reinterpret_cast<void *>(WebPDecoder::nativeDecodeFrames)
        },
        {
                "nativeDecodeNextFrame",
                "()Lcom/aureusapps/android/webpandroid/decoder/FrameDecodeResult;",
                reinterpret_cast<void *>(WebPDecoder::nativeDecodeNextFrame)
        },
        {
                "nativeResetDecoder",
                "()V",
                reinterpret_cast<void *>(WebPDecoder::nativeResetDecoder)
        },
        {
                "nativeCancel",
                "()V",
                reinterpret_cast<void *>(WebPDecoder::nativeCancel)
        },
        {
                "nativeRelease",
                "()V",
                reinterpret_cast<void *>(WebPDecoder::nativeRelease)
        },
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