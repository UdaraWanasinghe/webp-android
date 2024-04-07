//
// Created by udara on 9/3/23.
//

#pragma once

#include <string>
#include <jni.h>
#include <webp/demux.h>

#include "result_codes.h"

namespace dec {
    struct DecoderConfig {
        std::string namePrefix = "IMG_";
        char repeatCharacter = '0';
        int repeatCharacterCount = 4;
        int compressFormatOrdinal = 1;
        int compressQuality = 100;
    };

    DecoderConfig parseDecoderConfig(JNIEnv *env, jobject jconfig);

    std::string getImageNameSuffix(int compress_format_ordinal);
}

typedef struct {
    ResultCode result_code;
    jobject webp_info;
} InfoDecodeResult;

typedef struct {
    ResultCode result_code;
    int timestamp;
} FrameDecodeResult;

class WebPDecoder {

private:
    dec::DecoderConfig decoder_config_{};
    WebPAnimDecoder *decoder_ = nullptr;
    bool cancel_flag_ = false;
    int frame_count_ = 0;
    int current_frame_index_ = 0;

    void configure(dec::DecoderConfig config);

    static jobject getWebPInfo(JNIEnv *env, const WebPBitstreamFeatures &features);

    static jobject decodeAnimInfo(
            JNIEnv *env,
            WebPAnimDecoder *decoder,
            const WebPBitstreamFeatures &features
    );

    static ResultCode processFrame(
            JNIEnv *env,
            jobject jdecoder,
            jobject jcontext,
            jobject jbitmap,
            jobject jdst_uri,
            uint8_t *pixels,
            int timestamp,
            int index
    );

    static ResultCode decodeAnimFrames(
            JNIEnv *env,
            jobject jdecoder,
            jobject jcontext,
            jobject jdst_uri,
            WebPBitstreamFeatures &features,
            const uint8_t *file_data,
            size_t file_size
    );

    static ResultCode decodeStaticFrame(
            JNIEnv *env,
            jobject jdecoder,
            jobject jcontext,
            jobject jdst_uri,
            const WebPBitstreamFeatures &features,
            const uint8_t *file_data,
            size_t file_size
    );

    static ResultCode notifyInfoDecoded(JNIEnv *env, jobject jdecoder, jobject jinfo);

public:
    jobject webPData = nullptr;
    jobject bitmapFrame = nullptr;

    static WebPDecoder *getInstance(JNIEnv *env, jobject jdecoder);

    static jlong nativeCreate(JNIEnv *env, jobject thiz);

    static void nativeConfigure(JNIEnv *env, jobject thiz, jobject jconfig);

    static jint nativeSetDataSource(JNIEnv *env, jobject jdecoder, jobject jcontext, jobject jsrc_uri);

    static jint nativeDecodeFrames(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jobject jsrc_uri,
            jobject jdst_uri
    );

    static jobject nativeDecodeInfo(JNIEnv *env, jobject jdecoder);

    static jobject
    nativeDecodeInfo2(JNIEnv *env, jobject jdecoder, jobject jcontext, jobject jsrc_uri);

    static jobject nativeDecodeNextFrame(JNIEnv *env, jobject jdecoder);

    static void nativeResetDecoder(JNIEnv *env, jobject jdecoder);

    static void nativeCancel(JNIEnv *env, jobject jdecoder);

    static void nativeRelease(JNIEnv *env, jobject jdecoder);

    ResultCode setDecoderData(const uint8_t *file_data, size_t file_size);

    InfoDecodeResult decodeWebPInfo(JNIEnv *env);

    FrameDecodeResult decodeNextFrame(JNIEnv *env, jobject jbitmap);

    void resetDecoder();
};