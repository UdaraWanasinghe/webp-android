//
// Created by udara on 11/3/21.
//

#pragma once

#include <jni.h>
#include <webp/encode.h>
#include <webp/mux.h>

namespace enc {
    /**
     * Parses the WebPPreset enum value from a Java preset enum.
     *
     * @param env Pointer to the JNI environment.
     * @param jpreset Pointer to the Java preset enum representing the WebPPreset.
     *
     * @return The parsed WebPPreset enum value.
     */
    WebPPreset parseWebPPreset(
            JNIEnv *env,
            jobject jpreset
    );

    /**
     * Applies java webp config to the given WebPConfig.
     *
     * @param env Pointer to the JNI environment.
     * @param jconfig Pointer to the Java config object representing the WebPConfig.
     * @param config Pointer to the WebPConfig struct to be populated.
     */
    void applyWebPConfig(
            JNIEnv *env,
            jobject jconfig,
            WebPConfig *config
    );

    /**
     * Parses the WebP quality value from a Java webp config object.
     *
     * @param env Pointer to the JNI environment.
     * @param jconfig Pointer to the Java object representing the WebP configuration.
     *
     * @return The parsed WebP quality value as a float.
     */
    float parseWebPQuality(
            JNIEnv *env,
            jobject jconfig
    );

    /**
     * Parses the encoder options for WebP animation encoding.
     *
     * @param env A pointer to the JNI environment.
     * @param joptions A pointer to the Java object representing the options.
     * @param options A pointer to the WebP animation encoder options structure.
     */
    void parseEncoderOptions(
            JNIEnv *env,
            jobject joptions,
            WebPAnimEncoderOptions *options
    );

    /**
     * Copies pixels from the source buffer to the WebPPicture.
     *
     * @param src Pointer to the source buffer containing the pixel data.
     * @param pic Pointer to the WebPPicture object.
     */
    void copyPixels(
            const uint8_t *src,
            WebPPicture *pic
    );
}