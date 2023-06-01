//
// Created by udara on 11/5/21.
//

#define LOG_TAG "WEBP_DECODER"

#include <android/log.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <webp/decode.h>
#include <android/bitmap.h>

#include "webp/decode.h"
#include "webp/demux.h"
#include "include/jni_helper.h"
#include "include/webp_decoder_helper.h"

enum StatusFlag {
    START,
    STOP
};

class JNIWebPDecoder {

public:
    StatusFlag status_flag = START;
    WebPDecoderConfig decoderConfig{};
    WebPAnimDecoder *anim_decoder = nullptr;
    jweak decoder_obj = nullptr;
    bool decoderConfigured = false;

    JNIWebPDecoder() {
        // check path is file or url
        // create thread
        // load data to byte buffer
        // decode byte buffer
        // render frame with timestamp
    }

    static JNIWebPDecoder *GetInstance(
            JNIEnv *env,
            jobject thiz
    ) {
        jclass cls = env->GetObjectClass(thiz);
        if (!cls)
            ThrowException(env, "GetObjectClass failed");
        jfieldID nativeObjectPointerID = env->GetFieldID(cls, "nativeObjectPointer", "J");
        if (!nativeObjectPointerID)
            ThrowException(env, "GetFieldID failed");
        jlong nativeObjectPointer = env->GetLongField(thiz, nativeObjectPointerID);
        return reinterpret_cast<JNIWebPDecoder *>(nativeObjectPointer);
    }

    void DecodeFile(
            JNIEnv *env,
            char *path,
            WebPAnimDecoderOptions *decoder_options
    ) {

        // read webp file
        FILE *file;
        long file_size;
        uint8_t *buffer;
        size_t result;

        file = fopen(path, "rb");
        if (file == nullptr) {
            ThrowException(env, "Cannot open the file");
            return;
        }

        // obtain file size:
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        // allocate memory to contain the whole file:
        buffer = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * file_size));
        if (buffer == nullptr) {
            ThrowException(env, "Memory error");
            return;
        }

        // copy the file into the buffer:
        result = fread(buffer, 1, file_size, file);
        if (result != file_size) {
            ThrowException(env, "Reading error");
            return;
        }

        // terminate
        fclose(file);

        // set data
        WebPData webp_data;
        WebPDataInit(&webp_data);
        webp_data.bytes = buffer;
        webp_data.size = file_size;

        // decode
        WebPAnimDecoder *decoder = WebPAnimDecoderNew(&webp_data, decoder_options);
        WebPAnimInfo anim_info;
        WebPAnimDecoderGetInfo(decoder, &anim_info);
        for (uint32_t i = 0; i < anim_info.loop_count; ++i) {
            while (WebPAnimDecoderHasMoreFrames(decoder)) {
                uint8_t *buf;
                int timestamp;
                WebPAnimDecoderGetNext(decoder, &buf, &timestamp);

            }
            WebPAnimDecoderReset(decoder);
        }
        WebPAnimDecoderDelete(decoder);

    }

    static const char *GetPath(
            JNIEnv *env,
            jobject self
    ) {
        // get path
        jclass decoder_class = env->GetObjectClass(self);
        jfieldID path_field_id = env->GetFieldID(decoder_class, "path", "Ljava/lang/String;");
        auto path_str = (jstring) env->GetObjectField(self, path_field_id);
        return env->GetStringUTFChars(path_str, nullptr);
    }
};

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_create(
        JNIEnv *,
        jobject
) {
    auto *decoder = new JNIWebPDecoder();
    return reinterpret_cast<jlong>(decoder);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_decode(
        JNIEnv *env,
        jobject,
        jstring filePath,
        jobject decodeListener
) {
    // load webp file data
    uint8_t *fileData;
    size_t fileSize;
    if (!loadFileData(env, filePath, &fileData, &fileSize)) {
        env->ThrowNew(
                env->FindClass("java/io/IOException"),
                "Error loading WebP file data."
        );
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
            "(Landroid/graphics/Bitmap;I)V"
    );

    // create webp info jobject
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

        // decode frame by frame
        while (WebPAnimDecoderGetNext(animDecoder, &pixelData, &timestamp)) {
            copyPixels(env, &pixelData, &bitmap);
            env->CallVoidMethod(decodeListener, onReceiveFrameMethodID, bitmap, timestamp);
        }
        WebPAnimDecoderReset(animDecoder);

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
        env->CallVoidMethod(decodeListener, onReceiveFrameMethodID, bitmap, 0);
        // release resources
        WebPFree(pixelData);
    }
    free(fileData);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_stop(
        JNIEnv *env,
        jobject self
) {
    auto *decoder = JNIWebPDecoder::GetInstance(env, self);
    decoder->status_flag = STOP;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_decoder_WebPDecoder_release(
        JNIEnv *env,
        jobject self
) {
    auto *decoder = JNIWebPDecoder::GetInstance(env, self);
    if (decoder != nullptr) {
        WebPAnimDecoderDelete(decoder->anim_decoder);
        jclass decoder_class = env->GetObjectClass(self);
        env->SetLongField(self,
                          env->GetFieldID(decoder_class, "nativeObjectPointer", "J"),
                          (jlong) 0);
        env->DeleteWeakGlobalRef(decoder->decoder_obj);
        delete decoder;
    } else {
        ThrowException(env, "JNIWebPDecoder is null");
    }
}