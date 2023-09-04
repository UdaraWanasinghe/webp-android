//
// Created by udara on 9/3/23.
//

#pragma once

#include <jni.h>
#include <webp/mux.h>

#include "result_codes.h"

class WebPAnimationEncoder {
private:
    static inline JavaVM *jvm = nullptr;
    inline static jweak progressObserver = nullptr;
    inline static bool cancelFlag = false;

    int imageWidth;
    int imageHeight;
    WebPAnimEncoderOptions encoderOptions{};
    int frameCount;
    WebPAnimEncoder *webPAnimEncoder;
    WebPConfig webPConfig{};

public:

    /**
     * Constructs a new instance of the WebPAnimationEncoder class with the specified width, height, and options.
     *
     * @param jvm The Java virtual machine pointer.
     * @param width The width of the animation frames in pixels.
     * @param height The height of the animation frames in pixels.
     * @param options The options for configuring the WebP animation encoder.
     */
    WebPAnimationEncoder(int width, int height, WebPAnimEncoderOptions options);

    /**
     * Configures the WebP encoder with the specified configuration.
     *
     * @param config The WebPConfig object containing the configuration settings.
     */
    void configure(WebPConfig config);

    /**
     * Adds a frame to the animation sequence with the specified pixel data, bitmap dimensions, and timestamp.
     *
     * @param pixels Pointer to the pixel data of the frame.
     * @param width The width of the frame bitmap in pixels.
     * @param height The height of the frame bitmap in pixels.
     * @param timestamp The timestamp of the frame in milliseconds.
     *
     * @return 0 if success or error code if failed.
     */
    ResultCode addFrame(uint8_t *pixels, int width, int height, long timestamp);

    /**
     * Assembles the animation with the provided timestamp and saves it to the specified output path.
     *
     * @param timestamp The timestamp to assign to the assembled animation in milliseconds.
     * @param webp_data Pointer to the output buffer for the WebP data.
     * @param webp_size Pointer to store the size of the WebP data.
     */
    ResultCode assemble(long timestamp, WebPData *data);

    /**
     * Release resources associated with this encoder.
     */
    void release();

    static WebPAnimationEncoder *getInstance(JNIEnv *env, jobject jencoder);

    static void setProgressNotifier(JNIEnv *env, jobject jencoder);

    static int notifyProgressChanged(int percent, const WebPPicture *picture);

    static void clearProgressNotifier(JNIEnv *env);

    static jlong nativeCreate(
            JNIEnv *env,
            jobject thiz,
            jint jwidth,
            jint jheight,
            jobject joptions
    );

    static void nativeConfigure(
            JNIEnv *env,
            jobject thiz,
            jobject jconfig,
            jobject jpreset
    );

    static void nativeAddFrame(
            JNIEnv *env,
            jobject thiz,
            jlong jtimestamp,
            jobject jsrc_bitmap
    );

    static void nativeAssemble(
            JNIEnv *env,
            jobject thiz,
            jobject jcontext,
            jlong jtimestamp,
            jobject jdst_uri
    );

    static void nativeCancel(
            JNIEnv *env,
            jobject thiz
    );

    static void nativeRelease(
            JNIEnv *env,
            jobject thiz
    );
};