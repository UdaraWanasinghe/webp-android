//
// Created by udara on 9/3/23.
//

#include "include/native_loader.h"
#include "include/webp_encoder.h"
#include "include/webp_anim_encoder.h"
#include "include/webp_decoder.h"

namespace {
    jclass getClass(JNIEnv *env, const char *name) {
        jclass clazz = env->FindClass(name);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return static_cast<jclass>(env->NewGlobalRef(clazz));
    }

    jfieldID getFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
        jfieldID fieldID = env->GetFieldID(clazz, name, sig);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return fieldID;
    }

    jfieldID getStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
        jfieldID fieldID = env->GetStaticFieldID(clazz, name, sig);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return fieldID;
    }

    jmethodID getMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
        jmethodID methodID = env->GetMethodID(clazz, name, sig);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return methodID;
    }

    jmethodID getStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
        jmethodID methodID = env->GetStaticMethodID(clazz, name, sig);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        return methodID;
    }
}

void ClassRegistry::initialize(JNIEnv *env) {
    // classes
    booleanClass = getClass(env, "java/lang/Boolean");
    integerClass = getClass(env, "java/lang/Integer");
    floatClass = getClass(env, "java/lang/Float");
    encoderClass = getClass(env, "com/aureusapps/android/webpandroid/encoder/WebPEncoder");
    animEncoderClass = getClass(env, "com/aureusapps/android/webpandroid/encoder/WebPAnimEncoder");
    decoderClass = getClass(env, "com/aureusapps/android/webpandroid/decoder/WebPDecoder");
    webPConfigClass = getClass(env, "com/aureusapps/android/webpandroid/encoder/WebPConfig");
    decoderConfigClass = getClass(env, "com/aureusapps/android/webpandroid/decoder/DecoderConfig");
    webPPresetClass = getClass(env, "com/aureusapps/android/webpandroid/encoder/WebPPreset");
    webPInfoClass = getClass(env, "com/aureusapps/android/webpandroid/decoder/WebPInfo");
    bitmapClass = getClass(env, "android/graphics/Bitmap");
    bitmapConfigClass = getClass(env, "android/graphics/Bitmap$Config");
    bitmapUtilsClass = getClass(env, "com/aureusapps/android/extensions/BitmapUtils");
    bitmapCompressFormatClass = getClass(env, "android/graphics/Bitmap$CompressFormat");
    runtimeExceptionClass = getClass(env, "java/lang/RuntimeException");
    cancellationExceptionClass = getClass(env, "java/util/concurrent/CancellationException");
    parcelFileDescriptorClass = getClass(env, "android/os/ParcelFileDescriptor");
    uriClass = getClass(env, "android/net/Uri");
    uriExtensionsClass = getClass(env, "com/aureusapps/android/extensions/UriExtensionsKt");
    contextClass = getClass(env, "android/content/Context");
    contentResolverClass = getClass(env, "android/content/ContentResolver");

    // fields
    encoderPointerFieldID = getFieldID(env, encoderClass, "nativePointer", "J");
    animEncoderPointerFieldID = getFieldID(env, animEncoderClass, "nativePointer", "J");
    decoderPointerFieldID = getFieldID(env, decoderClass, "nativePointer", "J");
    uriEmptyFieldID = getStaticFieldID(env, uriClass, "EMPTY", "Landroid/net/Uri;");
    bitmapConfigARGB8888FieldID = getStaticFieldID(
            env,
            bitmapConfigClass,
            "ARGB_8888",
            "Landroid/graphics/Bitmap$Config;"
    );
    compressFormatJPEGFieldID = getStaticFieldID(
            env,
            bitmapCompressFormatClass,
            "JPEG",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatPNGFieldID = getStaticFieldID(
            env,
            bitmapCompressFormatClass,
            "PNG",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatWEBPFieldID = getStaticFieldID(
            env,
            bitmapCompressFormatClass,
            "WEBP",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatWEBPLossyFieldID = getStaticFieldID(
            env,
            bitmapCompressFormatClass,
            "WEBP_LOSSY",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );
    compressFormatWEBPLosslessFieldID = getStaticFieldID(
            env,
            bitmapCompressFormatClass,
            "WEBP_LOSSLESS",
            "Landroid/graphics/Bitmap$CompressFormat;"
    );

    // methods
    booleanValueMethodID = getMethodID(env, booleanClass, "booleanValue", "()Z");
    integerValueMethodID = getMethodID(env, integerClass, "intValue", "()I");
    floatValueMethodID = getMethodID(env, floatClass, "floatValue", "()F");
    encoderNotifyProgressMethodID = getMethodID(
            env,
            encoderClass,
            "notifyProgressChanged",
            "(I)Z"
    );
    animEncoderNotifyProgressMethodID = getMethodID(
            env,
            animEncoderClass,
            "notifyProgressChanged",
            "(II)Z"
    );
    webPInfoConstructorID = getMethodID(env, webPInfoClass, "<init>", "(IIZZIII)V");
    decoderNotifyInfoDecodedMethodID = getMethodID(
            env,
            decoderClass,
            "notifyInfoDecoded",
            "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
    );
    decoderNotifyFrameDecodedMethodID = getMethodID(
            env,
            decoderClass,
            "notifyFrameDecoded",
            "(IJLandroid/graphics/Bitmap;Landroid/net/Uri;)V"
    );
    bitmapCreateMethodID = getStaticMethodID(
            env,
            bitmapClass,
            "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;"
    );
    bitmapCreateScaledMethodID = getStaticMethodID(
            env,
            bitmapClass,
            "createScaledBitmap",
            "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"
    );
    bitmapUtilsSaveInDirectoryMethodID = getStaticMethodID(
            env,
            bitmapUtilsClass,
            "saveInDirectory",
            "(Landroid/content/Context;Landroid/graphics/Bitmap;Landroid/net/Uri;Ljava/lang/String;Landroid/graphics/Bitmap$CompressFormat;I)Landroid/net/Uri;"
    );
    bitmapRecycleMethodID = getMethodID(
            env,
            bitmapClass,
            "recycle",
            "()V"
    );
    contextGetContentResolverMethodID = getMethodID(
            env,
            contextClass,
            "getContentResolver",
            "()Landroid/content/ContentResolver;"
    );
    contentResolverOpenFileDescriptorMethodID = getMethodID(
            env,
            contentResolverClass,
            "openFileDescriptor",
            "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;"
    );
    parcelFileDescriptorGetFdMethodID = getMethodID(
            env,
            parcelFileDescriptorClass,
            "getFd",
            "()I"
    );
    parcelFileDescriptorCloseMethodID = getMethodID(
            env,
            parcelFileDescriptorClass,
            "close",
            "()V"
    );
    parcelFileDescriptorCloseWithErrorMethodID = getMethodID(
            env,
            parcelFileDescriptorClass,
            "closeWithError",
            "(Ljava/lang/String;)V"
    );
    uriExtensionsReadToBufferMethodID = getStaticMethodID(
            env,
            uriExtensionsClass,
            "readToBuffer",
            "(Landroid/net/Uri;Landroid/content/Context;)Ljava/nio/ByteBuffer;"
    );
    uriExtensionsFileExistsMethodID = getStaticMethodID(
            env,
            uriExtensionsClass,
            "fileExists",
            "(Landroid/net/Uri;Landroid/content/Context;Ljava/lang/String;)Z"
    );
}

void ClassRegistry::release(JNIEnv *env) {
    env->DeleteGlobalRef(booleanClass);
    env->DeleteGlobalRef(integerClass);
    env->DeleteGlobalRef(floatClass);
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