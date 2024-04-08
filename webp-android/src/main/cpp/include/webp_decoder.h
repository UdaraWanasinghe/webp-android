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

    jlong nativeCreate(JNIEnv *env, jobject thiz);

    void nativeConfigure(JNIEnv *env, jobject jdecoder, jobject jconfig);

    jint nativeSetDataSource(JNIEnv *env, jobject jdecoder, jobject jcontext, jobject jsrc_uri);

    jobject nativeDecodeInfo(JNIEnv *env, jobject jdecoder);

    jobject nativeDecodeNextFrame(JNIEnv *env, jobject jdecoder);

    jint nativeDecodeFrames(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jobject jdst_uri
    );

    void nativeReset(JNIEnv *env, jobject jdecoder);

    void nativeCancel(JNIEnv *env, jobject jdecoder);

    void nativeRelease(JNIEnv *env, jobject jdecoder);
}

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

public:
    static WebPDecoder *getInstance(JNIEnv *env, jobject jdecoder);

    void configure(dec::DecoderConfig *config);

    ResultCode setDataSource(JNIEnv *env, jobject jbyte_buffer);

    dec::InfoDecodeResult decodeWebPInfo(JNIEnv *env);

    dec::FrameDecodeResult decodeNextFrame(JNIEnv *env);

    ResultCode decodeFrames(
            JNIEnv *env,
            jobject jdecoder,
            jobject jcontext,
            jobject jdst_uri
    );

    void cancel();

    void reset();

    void fullReset(JNIEnv *env);
};