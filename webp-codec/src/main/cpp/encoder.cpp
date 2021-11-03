//
// Created by udara on 11/2/21.
//

#include <android/bitmap.h>

#include "includes/encoder.h"
#include "includes/helper.h"
#include "webp/encode.h"
#include "webp/mux.h"

class Encoder {

public:
    WebPAnimEncoder *encoder = nullptr;
    WebPConfig *config = nullptr;

    Encoder(int width, int height, WebPAnimEncoderOptions *options) {
        encoder = WebPAnimEncoderNew(width, height, options);
    }

    static Encoder *GetInstance(JNIEnv *env, jobject self) {
        jclass cls = env->GetObjectClass(self);
        if (!cls)
            ThrowException(env, "GetObjectClass failed");
        jfieldID nativeObjectPointerID = env->GetFieldID(cls, "nativeObjectPointer", "J");
        if (!nativeObjectPointerID)
            ThrowException(env, "GetFieldID failed");
        jlong nativeObjectPointer = env->GetLongField(self, nativeObjectPointerID);
        return reinterpret_cast<Encoder *>(nativeObjectPointer);
    }

    static void ParseOptions(JNIEnv *env, jobject options, WebPAnimEncoderOptions *encoder_options) {
        if (options != nullptr) {
            jclass options_class = env->GetObjectClass(options);
            // minimizeSize
            jobject minimize_size = env->GetObjectField(options,
                                                        env->GetFieldID(options_class, "minimizeSize",
                                                                        "Ljava/lang/Boolean;"));
            if (minimize_size != nullptr) {
                encoder_options->minimize_size = GetBooleanValue(env, minimize_size);
            }
            // kmin
            jobject kmin = env->GetObjectField(options, env->GetFieldID(options_class, "kmin", "Ljava/lang/Integer;"));
            if (kmin != nullptr) {
                encoder_options->kmin = GetIntegerValue(env, kmin);
            }
            // kmax
            jobject kmax = env->GetObjectField(options, env->GetFieldID(options_class, "kmax", "Ljava/lang/Integer;"));
            if (kmax != nullptr) {
                encoder_options->kmax = GetIntegerValue(env, kmax);
            }
            // allowMixed
            jobject allow_mixed = env->GetObjectField(options,
                                                      env->GetFieldID(options_class, "allowMixed", "Ljava/lang/Boolean;"));
            if (allow_mixed != nullptr) {
                encoder_options->allow_mixed = GetBooleanValue(env, allow_mixed);
            }
            // verbose
            jobject verbose = env->GetObjectField(options, env->GetFieldID(options_class, "verbose", "Ljava/lang/Boolean;"));
            if (verbose != nullptr) {
                encoder_options->verbose = GetBooleanValue(env, verbose);
            }
            // anim params
            WebPMuxAnimParams anim_params;
            jobject anim_params_obj = env->GetObjectField(options, env->GetFieldID(options_class, "animParams",
                                                                                   "Lcom/aureusapps/webpcodec/encoder/WebPMuxAnimParams"));
            if (anim_params_obj != nullptr) {
                jclass anim_params_class = env->GetObjectClass(anim_params_obj);
                // bgColor
                jobject background_color = env->GetObjectField(anim_params_obj,
                                                               env->GetFieldID(anim_params_class, "backgroundColor",
                                                                               "Ljava/lang/Integer;"));
                if (background_color != nullptr) {
                    anim_params.bgcolor = GetIntegerValue(env, background_color);
                } else {
                    anim_params.bgcolor = 0;
                }
                // loopCount
                jobject loop_count = env->GetObjectField(anim_params_obj,
                                                         env->GetFieldID(anim_params_class, "loopCount",
                                                                         "Ljava/lang/Integer;"));
                if (loop_count != nullptr) {
                    anim_params.loop_count = GetIntegerValue(env, loop_count);
                } else {
                    anim_params.loop_count = 1;
                }
            } else {
                anim_params.bgcolor = 0;
                anim_params.loop_count = 1;
            }
            encoder_options->anim_params = anim_params;
        }
    }

    static void ParseConfig(JNIEnv *env, jobject config, WebPConfig *encoder_config) {
        if (config != nullptr) {
            // config
            jclass config_class = env->GetObjectClass(config);
            // lossless
            jobject lossless = env->GetObjectField(config, env->GetFieldID(config_class, "lossless", "Ljava/lang/Integer;"));
            if (lossless != nullptr) {
                encoder_config->lossless = GetIntegerValue(env, config);
            }
            // quality
            jobject quality = env->GetObjectField(config, env->GetFieldID(config_class, "quality", "Ljava/lang/Float;"));
            if (quality != nullptr) {
                encoder_config->quality = GetFloatValue(env, quality);
            }
            // method
            jobject method = env->GetObjectField(config,
                                                 env->GetFieldID(config_class, "method", "Ljava/lang/Integer;"));
            if (method != nullptr) {
                encoder_config->target_size = GetIntegerValue(env, method);
            }
            // target_size
            jobject target_size = env->GetObjectField(config, env->GetFieldID(config_class, "targetSize",
                                                                              "Ljava/lang/Integer;"));
            if (target_size != nullptr) {
                encoder_config->target_size = GetIntegerValue(env, target_size);
            }
            // target_PSNR
            jobject target_PSNR = env->GetObjectField(config,
                                                      env->GetFieldID(config_class, "targetPSNR", "Ljava/lang/Float;"));
            if (target_PSNR != nullptr) {
                encoder_config->target_PSNR = GetIntegerValue(env, target_PSNR);
            }
            // segments
            jobject segments = env->GetObjectField(config,
                                                   env->GetFieldID(config_class, "segments", "Ljava/lang/Integer;"));
            if (segments != nullptr) {
                encoder_config->segments = GetIntegerValue(env, segments);
            }
            // snsStrength
            jobject sns_strength = env->GetObjectField(config, env->GetFieldID(config_class, "snsStrength",
                                                                               "Ljava/lang/Integer;"));
            if (sns_strength != nullptr) {
                encoder_config->sns_strength = GetIntegerValue(env, sns_strength);
            }
            // filterStrength
            jobject filter_strength = env->GetObjectField(config, env->GetFieldID(config_class, "filterStrength",
                                                                                  "Ljava/lang/Integer;"));
            if (filter_strength != nullptr) {
                encoder_config->filter_strength = GetIntegerValue(env, filter_strength);
            }
            // filterSharpness
            jobject filter_sharpness = env->GetObjectField(config, env->GetFieldID(config_class, "filterSharpness",
                                                                                   "Ljava/lang/Integer;"));
            if (filter_sharpness != nullptr) {
                encoder_config->filter_sharpness = GetIntegerValue(env, filter_sharpness);
            }
            // filterType
            jobject filter_type = env->GetObjectField(config, env->GetFieldID(config_class, "filterType",
                                                                              "Ljava/lang/Integer;"));
            if (filter_type != nullptr) {
                encoder_config->filter_type = GetIntegerValue(env, filter_type);
            }
            // autoFilter
            jobject auto_filter = env->GetObjectField(config, env->GetFieldID(config_class, "autoFilter",
                                                                              "Ljava/lang/Boolean;"));
            if (auto_filter != nullptr) {
                encoder_config->autofilter = GetBooleanValue(env, auto_filter);
            }
            // alphaCompression
            jobject alpha_compression = env->GetObjectField(config,
                                                            env->GetFieldID(config_class, "alphaCompression",
                                                                            "Ljava/lang/Integer;"));
            if (alpha_compression != nullptr) {
                encoder_config->alpha_compression = GetIntegerValue(env, alpha_compression);
            }
            // alphaFiltering
            jobject alpha_filtering = env->GetObjectField(config, env->GetFieldID(config_class, "alphaFiltering",
                                                                                  "Ljava/lang/Integer;"));
            if (alpha_filtering != nullptr) {
                encoder_config->alpha_filtering = GetIntegerValue(env, alpha_filtering);
            }

            // alphaQuality
            jobject alpha_quality = env->GetObjectField(config, env->GetFieldID(config_class, "alphaQuality",
                                                                                "Ljava/lang/Integer;"));
            if (alpha_quality != nullptr) {
                encoder_config->alpha_quality = GetIntegerValue(env, alpha_quality);
            }
            // pass
            jobject pass = env->GetObjectField(config, env->GetFieldID(config_class, "pass",
                                                                       "Ljava/lang/Integer;"));
            if (pass != nullptr) {
                encoder_config->pass = GetIntegerValue(env, pass);
            }
            // showCompressed
            jobject show_compressed = env->GetObjectField(config, env->GetFieldID(config_class, "showCompressed",
                                                                                  "Ljava/lang/Boolean;"));
            if (show_compressed != nullptr) {
                encoder_config->show_compressed = GetBooleanValue(env, show_compressed);
            }
            // preprocessing
            jobject preprocessing = env->GetObjectField(config, env->GetFieldID(config_class, "preprocessing",
                                                                                "Ljava/lang/Integer;"));
            if (preprocessing != nullptr) {
                encoder_config->preprocessing = GetIntegerValue(env, preprocessing);
            }
            // partitions
            jobject partitions = env->GetObjectField(config, env->GetFieldID(config_class, "partitions",
                                                                             "Ljava/lang/Integer;"));
            if (preprocessing != nullptr) {
                encoder_config->partitions = GetIntegerValue(env, partitions);
            }
            // partitionLimit
            jobject partition_limit = env->GetObjectField(config, env->GetFieldID(config_class, "partitionLimit",
                                                                                  "Ljava/lang/Integer;"));
            if (partition_limit != nullptr) {
                encoder_config->partition_limit = GetIntegerValue(env, partition_limit);
            }
            // emulateJPEGSize
            jobject emulate_jpeg_size = env->GetObjectField(config, env->GetFieldID(config_class, "emulateJPEGSize",
                                                                                    "Ljava/lang/Boolean;"));
            if (emulate_jpeg_size != nullptr) {
                encoder_config->emulate_jpeg_size = GetBooleanValue(env, emulate_jpeg_size);
            }
            // threadLevel
            jobject thread_level = env->GetObjectField(config, env->GetFieldID(config_class, "threadLevel",
                                                                               "Ljava/lang/Integer;"));
            if (thread_level != nullptr) {
                encoder_config->thread_level = GetIntegerValue(env, thread_level);
            }
            // lowMemory
            jobject low_memory = env->GetObjectField(config, env->GetFieldID(config_class, "lowMemory",
                                                                             "Ljava/lang/Boolean;"));
            if (low_memory != nullptr) {
                encoder_config->low_memory = GetBooleanValue(env, low_memory);
            }
            // nearLossless
            jobject near_lossless = env->GetObjectField(config, env->GetFieldID(config_class, "nearLossless",
                                                                                "Ljava/lang/Integer;"));
            if (near_lossless != nullptr) {
                encoder_config->near_lossless = GetIntegerValue(env, near_lossless);
            }
            // exact
            jobject exact = env->GetObjectField(config, env->GetFieldID(config_class, "exact",
                                                                        "Ljava/lang/Boolean;"));
            if (exact != nullptr) {
                encoder_config->exact = GetBooleanValue(env, exact);
            }
            // useDeltaPalette
            jobject use_delta_palette = env->GetObjectField(config, env->GetFieldID(config_class, "useDeltaPalette",
                                                                                    "Ljava/lang/Boolean;"));
            if (use_delta_palette != nullptr) {
                encoder_config->use_delta_palette = GetBooleanValue(env, use_delta_palette);
            }
            // useSharpYUV
            jobject use_sharp_yuv = env->GetObjectField(config, env->GetFieldID(config_class, "useSharpYUV",
                                                                                "Ljava/lang/Boolean;"));
            if (use_sharp_yuv != nullptr) {
                encoder_config->use_sharp_yuv = GetBooleanValue(env, use_sharp_yuv);
            }
            // qmin
            jobject qmin = env->GetObjectField(config, env->GetFieldID(config_class, "qmin",
                                                                       "Ljava/lang/Integer;"));
            if (qmin != nullptr) {
                encoder_config->qmin = GetIntegerValue(env, qmin);
            }
            // qmax
            jobject qmax = env->GetObjectField(config, env->GetFieldID(config_class, "qmax",
                                                                       "Ljava/lang/Integer;"));
            if (qmax != nullptr) {
                encoder_config->qmax = GetIntegerValue(env, qmax);
            }
        }
    }

    static WebPPreset ParsePreset(JNIEnv *env, jobject preset) {
        jclass preset_class = env->GetObjectClass(preset);
        int preset_value = env->GetIntField(preset, env->GetFieldID(preset_class, "value", "I"));
        return static_cast<WebPPreset>(preset_value);
    }
};

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_create(JNIEnv *env, jobject, jint width, jint height,
                                                             jobject options) {

    WebPAnimEncoderOptions encoder_options;
    if (WebPAnimEncoderOptionsInit(&encoder_options)) {
        Encoder::ParseOptions(env, options, &encoder_options);
        auto *encoder = new Encoder(width, height, &encoder_options);
        return reinterpret_cast<jlong>(encoder);
    } else {
        ThrowException(env, "WebPAnimEncoderOptionsInit failed");
    }
    return 0;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_config__Lcom_aureusapps_webpcodec_encoder_WebPConfig_2(JNIEnv *env,
                                                                                                             jobject thiz,
                                                                                                             jobject config) {
    WebPConfig encoder_config;
    if (WebPConfigInit(&encoder_config)) {
        Encoder::ParseConfig(env, config, &encoder_config);
        Encoder *encoder = Encoder::GetInstance(env, thiz);
        encoder->config = &encoder_config;
        return JNI_TRUE;
    } else {
        ThrowException(env, "WebPConfigInit failed");
    }
    return JNI_FALSE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_config__Lcom_aureusapps_webpcodec_encoder_WebPPreset_2F(JNIEnv *env,
                                                                                                              jobject thiz,
                                                                                                              jobject preset,
                                                                                                              jfloat quality) {
    WebPPreset encoder_preset = Encoder::ParsePreset(env, preset);
    WebPConfig encoder_config;
    if (WebPConfigPreset(&encoder_config, encoder_preset, quality)) {
        Encoder *encoder = Encoder::GetInstance(env, thiz);
        encoder->config = &encoder_config;
        return JNI_TRUE;
    } else {
        ThrowException(env, "WebPConfigPreset failed");
    }
    return JNI_FALSE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_addFrame(JNIEnv *env, jobject thiz, jobject frame) {
    jclass frame_class = env->GetObjectClass(frame);
    jobject bitmap = env->GetObjectField(frame, env->GetFieldID(frame_class, "bitmap", "Landroid/graphics/Bitmap;"));
    jlong timestamp = env->GetLongField(frame, env->GetFieldID(frame_class, "timestamp", "J"));

    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        return JNI_FALSE;
    }

    void *pixels;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        return JNI_FALSE;
    }

    WebPPicture webp_picture;
    if (WebPPictureInit(&webp_picture)) {
        webp_picture.use_argb = JNI_TRUE;
        webp_picture.width = info.width;
        webp_picture.height = info.height;
        webp_picture.colorspace = WEBP_YUV420;
        webp_picture.argb_stride = 4;
        if (WebPPictureAlloc(&webp_picture)) {
            // copy pixels from bitmap
            uint32_t *dst = webp_picture.argb;
            for (int y = 0; y < info.height; ++y) {
                memcpy(dst, pixels, info.width * 4);
                pixels = (int *) pixels + info.stride;
                dst += webp_picture.argb_stride;
            }
            AndroidBitmap_unlockPixels(env, bitmap);
            auto *encoder = Encoder::GetInstance(env, thiz);
            if (WebPAnimEncoderAdd(encoder->encoder, &webp_picture, timestamp, encoder->config)) {
                return JNI_TRUE;
            } else {
                ThrowException(env, "WebPAnimEncoderAdd failed");
            }
        } else {
            ThrowException(env, "WebPPictureAlloc failed");
        }
    } else {
        ThrowException(env, "WebPPictureInit failed");
    }
    return JNI_FALSE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_assemble(JNIEnv *env, jobject thiz, jlong timestamp, jstring path) {
    // TODO: implement assemble()
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_release(JNIEnv *env, jobject thiz) {
    // TODO: implement release()
}