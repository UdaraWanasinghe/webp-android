//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>
#include <webp/encode.h>

#include "result_codes.h"

class WebPEncoder {

private:
    inline static JavaVM *jvm = nullptr;
    inline static jweak progressObserver = nullptr;
    inline static bool cancelFlag = false;

    int imageWidth;
    int imageHeight;
    WebPConfig webPConfig{};

public:
    /**
     * Constructs a WebPEncoder object with the specified width and height.
     *
     * @param width The width of the output image.
     * @param height The height of the output image.
     */
    WebPEncoder(int width, int height);

    /**
     * Configures the WebP encoding parameters.
     *
     * @param config The WebP configuration.
     */
    void configure(WebPConfig config);

    /**
     * Encodes the image into WebP format.
     *
     * @param pixels Pointer to the input pixel data from Android bitmap.
     * @param image_width The width of the input image.
     * @param image_height The height of the input image.
     * @param output_width The width the output image.
     * @param output_height The height of the output image.
     * @param webp_data Pointer to the output buffer for the WebP data.
     * @param webp_size Pointer to store the size of the WebP data.
     *
     * @return 0 if success, otherwise error code.
     */
    ResultCode encode(
            const uint8_t *pixels,
            int image_width,
            int image_height,
            int output_width,
            int output_height,
            const uint8_t **webp_data,
            size_t *webp_size
    );

    /**
    * Releases any resources held by the WebPEncoder object.
    */
    void release();

    static WebPEncoder *getInstance(JNIEnv *env, jobject jencoder);

    static void setProgressNotifier(JNIEnv *env, jobject jencoder);

    static int notifyProgressChanged(int percent, const WebPPicture *);

    static void clearProgressNotifier(JNIEnv *env);

    static jlong nativeCreate(
            JNIEnv *env,
            jobject thiz,
            jint jwidth,
            jint jheight
    );

    static void nativeConfigure(
            JNIEnv *env,
            jobject thiz,
            jobject jconfig,
            jobject jpreset
    );

    static void nativeEncode(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jobject jsrc_bitmap,
            jobject jdst_uri
    );

    static void nativeCancel(
            JNIEnv *env,
            jobject
    );

    static void nativeRelease(
            JNIEnv *env,
            jobject thiz
    );
};