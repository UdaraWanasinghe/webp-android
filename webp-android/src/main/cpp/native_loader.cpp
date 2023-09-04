//
// Created by udara on 9/3/23.
//

#include <jni.h>

#include "include/native_loader.h"
#include "include/webp_encoder.h"
#include "include/webp_anim_encoder.h"
#include "include/webp_decoder.h"

void ClassRegistry::initialize(JNIEnv *env) {
    // classes
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

    // fields
    encoderPointerFieldID = env->GetFieldID(encoderClass, "nativePointer", "J");
    animEncoderPointerFieldID = env->GetFieldID(animEncoderClass, "nativePointer", "J");
    decoderPointerFieldID = env->GetFieldID(decoderClass, "nativePointer", "J");
    uriEmptyFieldID = env->GetStaticFieldID(uriClass, "EMPTY", "Landroid/net/Uri;");
    bitmapConfigARGB8888FieldID = env->GetStaticFieldID(
            bitmapConfigClass,
            "ARGB_8888",
            "Landroid/graphics/Bitmap$Config;"
    );
    compressFormatJPEGFieldID = env->GetStaticFieldID(
            bitmapCompressFormatClass,
            "JPEG",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatPNGFieldID = env->GetStaticFieldID(
            bitmapCompressFormatClass,
            "PNG",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatWEBPFieldID = env->GetStaticFieldID(
            bitmapCompressFormatClass,
            "WEBP",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatWEBPLossyFieldID = env->GetStaticFieldID(
            bitmapCompressFormatClass,
            "WEBP_LOSSY",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatWEBPLosslessFieldID = env->GetStaticFieldID(
            bitmapCompressFormatClass,
            "WEBP_LOSSLESS",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );

    // methods
    encoderNotifyProgressMethodID = env->GetMethodID(
            encoderClass,
            "notifyProgressChanged",
            "(I)Z"
    );
    animEncoderNotifyProgressMethodID = env->GetMethodID(
            animEncoderClass,
            "notifyProgressChanged",
            "(II)Z"
    );
    webPInfoConstructorID = env->GetMethodID(webPInfoClass, "<init>", "(IIZZIII)V");
    decoderNotifyInfoDecodedMethodID = env->GetMethodID(
            decoderClass,
            "notifyInfoDecoded",
            "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
    );
    decoderNotifyFrameDecodedMethodID = env->GetMethodID(
            decoderClass,
            "notifyFrameDecoded",
            "(IJLandroid/graphics/Bitmap;Landroid/net/Uri;)V"
    );
    bitmapCreateMethodID = env->GetStaticMethodID(
            bitmapClass,
            "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
    );
    bitmapUtilsDecodeUriMethodId = env->GetStaticMethodID(
            bitmapUtilsClass,
            "decodeUri",
            "(Landroid/content/Context;Landroid/net/Uri;)Landroid/graphics/Bitmap;"
    );
    bitmapCreateScaledMethodID = env->GetStaticMethodID(
            bitmapClass,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    bitmapUtilsSaveInDirectoryMethodID = env->GetStaticMethodID(
            bitmapUtilsClass,
            "saveInDirectory",
            "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
    );
    bitmapRecycleMethodID = env->GetMethodID(
            bitmapClass,
            "recycle",
            "()V"
    );
}

void ClassRegistry::release(JNIEnv *env) {
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
    ClassRegistry::initialize(env);

    // register native methods
    // encoder methods
    int result = env->RegisterNatives(
            ClassRegistry::encoderClass,
            encoderMethods,
            sizeof(encoderMethods) / sizeof(JNINativeMethod)
    );
    if (result != JNI_OK) return result;

    // anim encoder methods
    result = env->RegisterNatives(
            ClassRegistry::animEncoderClass,
            animEncoderMethods,
            sizeof(animEncoderMethods) / sizeof(JNINativeMethod)
    );
    if (result != JNI_OK) return result;

    // decoder methods
    result = env->RegisterNatives(
            ClassRegistry::decoderClass,
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