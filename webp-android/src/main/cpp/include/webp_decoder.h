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

class WebPDecoder {
private:
    inline static bool cancelFlag = false;

    dec::DecoderConfig decoderConfig{};

    void configure(dec::DecoderConfig config);

    static WebPDecoder *getInstance(JNIEnv *env, jobject jdecoder);

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
    static jlong nativeCreate(JNIEnv *env, jobject thiz);

    static void nativeConfigure(JNIEnv *env, jobject thiz, jobject jconfig);

    static void nativeDecodeFrames(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jobject jsrc_uri,
            jobject jdst_uri
    );

    static jobject nativeDecodeInfo(JNIEnv *env, jobject thiz, jobject jcontext, jobject jsrc_uri);

    static void nativeCancel(JNIEnv *env, jobject thiz);

    static void nativeRelease(JNIEnv *env, jobject thiz);
};