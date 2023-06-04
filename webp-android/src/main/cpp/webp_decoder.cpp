//
// Created by udara on 11/5/21.
//

#include <fstream>
#include <sstream>
#include <webp/demux.h>

#include "include/webp_decoder_helper.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_extractImages(
        JNIEnv *env,
        jobject,
        jstring filePath,
        jobject decodeListener
) {
    // load webp file data
    uint8_t *fileData;
    size_t fileSize;
    if (!loadFileData(env, filePath, &fileData, &fileSize)) {
        return;
    }

    // get webp file features
    WebPBitstreamFeatures features;
    WebPGetFeatures(fileData, fileSize, &features);

    // get decode listener
    jclass decodeListenerClass = env->GetObjectClass(decodeListener);
    jmethodID onReceiveInfoMethodID = env->GetMethodID(
            decodeListenerClass,
            "onReceiveInfo",
            "(Lcom/aureusapps/android/webpandroid/decoder/WebPInfo;)V"
    );
    jmethodID onReceiveFrameMethodID = env->GetMethodID(
            decodeListenerClass,
            "onReceiveFrame",
            "(Landroid/graphics/Bitmap;II)V"
    );

    // webp info
    jclass webPInfoClass = env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPInfo");
    jmethodID webPInfoConstructorID = env->GetMethodID(webPInfoClass, "<init>", "(IIZZIII)V");

    if (features.has_animation) {
        // use animation decoder
        // decode options
        WebPAnimDecoderOptions decoderOptions;
        WebPAnimDecoderOptionsInit(&decoderOptions);
        decoderOptions.color_mode = MODE_RGBA;
        decoderOptions.use_threads = true;

        // create decoder
        WebPData webPData;
        WebPDataInit(&webPData);
        webPData.size = fileSize;
        webPData.bytes = fileData;
        WebPAnimDecoder *animDecoder = WebPAnimDecoderNew(&webPData, &decoderOptions);

        // get extended info
        WebPAnimInfo animInfo;
        WebPAnimDecoderGetInfo(animDecoder, &animInfo);

        // submit extended info
        int frameWidth = static_cast<int>(animInfo.canvas_width);
        int frameHeight = static_cast<int>(animInfo.canvas_height);
        jobject webPInfo = env->NewObject(
                webPInfoClass,
                webPInfoConstructorID,
                frameWidth,
                frameHeight,
                features.has_alpha,
                features.has_animation,
                static_cast<int>(animInfo.bgcolor),
                static_cast<int>(animInfo.frame_count),
                static_cast<int>(animInfo.loop_count)
        );
        env->CallVoidMethod(decodeListener, onReceiveInfoMethodID, webPInfo);
        jobject bitmap = createBitmap(env, frameWidth, frameHeight);

        uint8_t *pixelData;
        int timestamp;
        int index = 0;

        // decode frame by frame
        while (WebPAnimDecoderGetNext(animDecoder, &pixelData, &timestamp)) {
            copyPixels(env, &pixelData, &bitmap);
            env->CallVoidMethod(decodeListener, onReceiveFrameMethodID, bitmap, index, timestamp);
            index++;
        }
        WebPAnimDecoderDelete(animDecoder);

    } else {
        // submit info
        jobject webPInfo = env->NewObject(
                webPInfoClass,
                webPInfoConstructorID,
                features.width,
                features.height,
                features.has_alpha,
                features.has_animation,
                0, 0, 0
        );
        env->CallVoidMethod(decodeListener, onReceiveInfoMethodID, webPInfo);

        // decode image
        int width;
        int height;
        uint8_t *pixelData = WebPDecodeRGBA(fileData, fileSize, &width, &height);
        // create bitmap
        jobject bitmap = createBitmap(env, width, height, &pixelData);
        // submit frame
        env->CallVoidMethod(decodeListener, onReceiveFrameMethodID, bitmap, 0, 0);
        // release resources
        WebPFree(pixelData);
    }
    free(fileData);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_readInfo(
        JNIEnv *env,
        jobject,
        jstring filePath
) {
    // load webp file data
    uint8_t *fileData;
    size_t fileSize;
    if (!loadFileData(env, filePath, &fileData, &fileSize)) {
        env->ThrowNew(
                env->FindClass("java/io/IOException"),
                "Error loading WebP file data."
        );
        return nullptr;
    }

    // get webp file features
    WebPBitstreamFeatures features;
    WebPGetFeatures(fileData, fileSize, &features);

    // webp info
    jclass webPInfoClass = env->FindClass("com/aureusapps/android/webpandroid/decoder/WebPInfo");
    jmethodID webPInfoConstructorID = env->GetMethodID(webPInfoClass, "<init>", "(IIZZIII)V");

    if (features.has_animation) {
        // use animation decoder
        // decode options
        WebPAnimDecoderOptions decoderOptions;
        WebPAnimDecoderOptionsInit(&decoderOptions);
        decoderOptions.color_mode = MODE_RGBA;
        decoderOptions.use_threads = true;

        // create decoder
        WebPData webPData;
        WebPDataInit(&webPData);
        webPData.size = fileSize;
        webPData.bytes = fileData;
        WebPAnimDecoder *animDecoder = WebPAnimDecoderNew(&webPData, &decoderOptions);

        // get extended info
        WebPAnimInfo animInfo;
        WebPAnimDecoderGetInfo(animDecoder, &animInfo);

        // return extended info
        int frameWidth = static_cast<int>(animInfo.canvas_width);
        int frameHeight = static_cast<int>(animInfo.canvas_height);
        jobject webPInfo = env->NewObject(
                webPInfoClass,
                webPInfoConstructorID,
                frameWidth,
                frameHeight,
                features.has_alpha,
                features.has_animation,
                static_cast<int>(animInfo.bgcolor),
                static_cast<int>(animInfo.frame_count),
                static_cast<int>(animInfo.loop_count)
        );
        WebPAnimDecoderDelete(animDecoder);
        free(fileData);
        return webPInfo;

    } else {
        // return info
        jobject webPInfo = env->NewObject(
                webPInfoClass,
                webPInfoConstructorID,
                features.width,
                features.height,
                features.has_alpha,
                features.has_animation,
                0, 0, 0
        );
        free(fileData);
        return webPInfo;
    }
}