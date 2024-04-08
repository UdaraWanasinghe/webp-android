//
// Created by udara on 9/3/23.
//

#pragma once

#include <string>
#include <jni.h>
#include <webp/demux.h>

#include "result_codes.h"

namespace dec {
    typedef struct DecoderConfig {
        std::string name_prefix = "IMG_";
        char repeat_character = '0';
        int repeat_character_count = 4;
        int compress_format_ordinal = 1;
        int compress_quality = 100;
    } DecoderConfig;

    DecoderConfig parseDecoderConfig(JNIEnv *env, jobject jconfig);

    std::string getImageNameSuffix(int compress_format_ordinal);
}

typedef struct {
    ResultCode result_code;
    jobject webp_info;
} InfoDecodeResult;

typedef struct {
    ResultCode result_code;
    int frame_index;
    jobject bitmap_frame;
    int timestamp;
} FrameDecodeResult;

class WebPDecoder {

private:
    dec::DecoderConfig decoder_config_{};
    bool cancel_flag_ = false;
    jobject data_buffer_ = nullptr;
    jobject bitmap_frame_ = nullptr;
    WebPAnimDecoder *decoder_ = nullptr;
    WebPBitstreamFeatures webp_features_ = {0};
    WebPAnimInfo anim_info_ = {0};
    int current_frame_index_ = 0;

    void configure(dec::DecoderConfig *config);

    static ResultCode notifyInfoDecoded(JNIEnv *env, jobject jdecoder, jobject jinfo);

public:
    static WebPDecoder *getInstance(JNIEnv *env, jobject jdecoder);

    static jlong nativeCreate(JNIEnv *env, jobject thiz);

    static void nativeConfigure(JNIEnv *env, jobject jdecoder, jobject jconfig);

    static jint nativeSetDataSource(JNIEnv *env, jobject jdecoder, jobject jcontext, jobject jsrc_uri);

    static jint nativeDecodeFrames(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jobject jdst_uri
    );

    static jobject nativeDecodeInfo(JNIEnv *env, jobject jdecoder);

    static jobject nativeDecodeNextFrame(JNIEnv *env, jobject jdecoder);

    static void nativeResetDecoder(JNIEnv *env, jobject jdecoder);

    static void nativeCancel(JNIEnv *env, jobject jdecoder);

    ResultCode setDecoderData(JNIEnv *env, jobject jbyte_buffer);

    InfoDecodeResult decodeWebPInfo(JNIEnv *env);

    FrameDecodeResult decodeNextFrame(JNIEnv *env);

    void resetDecoder();

    void fullResetDecoder(JNIEnv *env);

    static void nativeRelease(JNIEnv *env, jobject jdecoder);

};