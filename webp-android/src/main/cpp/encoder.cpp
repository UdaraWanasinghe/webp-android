//
// Created by udara on 11/2/21.
//

#include <cerrno>
#include <android/bitmap.h>

#include "includes/encoder.h"
#include "includes/helper.h"
#include "webp/encode.h"
#include "webp/mux.h"

static JavaVM *javaVm = nullptr;

class Encoder;

struct FrameUserData {
    Encoder *encoder;
    int frame_id;
};

class Encoder {

public:
    WebPAnimEncoder *anim_encoder = nullptr;
    WebPConfig encoder_config = {0};
    int frame_count = 0;
    jweak java_encoder_ref = nullptr;

    Encoder(int width, int height, WebPAnimEncoderOptions options, jweak java_encoder) {
        anim_encoder = WebPAnimEncoderNew(width, height, &options);
        java_encoder_ref = java_encoder;
    }

    static int OnProgressUpdate(int percent, const WebPPicture *picture) {
        JNIEnv *env;
        int env_stat = javaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
        switch (env_stat) {
            case JNI_EDETACHED:
                if (javaVm->AttachCurrentThread(&env, nullptr) != 0) {
                    // failed to attach
                    return true;
                }
                break;
            case JNI_EVERSION:
                // version not supported
                return true;
            case JNI_OK:
                break;
            default:
                return true;
        }
        auto *user_data = (FrameUserData *) picture->user_data;
        Encoder *encoder = user_data->encoder;
        int current_frame = user_data->frame_id;
        jweak encoder_ref = encoder->java_encoder_ref;
        jclass cls = env->GetObjectClass(encoder_ref);
        jmethodID method_id = env->GetMethodID(cls, "onProgressUpdate", "(II)V");
        env->CallVoidMethod(encoder_ref, method_id, percent, current_frame);
        return true;
    }

    static Encoder *GetInstance(JNIEnv *env, jobject self) {
        jclass cls = env->GetObjectClass(self);
        if (!cls) {
            ThrowException(env, "GetObjectClass failed");
        }
        jfieldID nativeObjectPointerID = env->GetFieldID(cls, "nativeObjectPointer", "J");
        if (!nativeObjectPointerID) {
            ThrowException(env, "GetFieldID failed");
        }
        jlong nativeObjectPointer = env->GetLongField(self, nativeObjectPointerID);
        return reinterpret_cast<Encoder *>(nativeObjectPointer);
    }

    static void
    ParseOptions(JNIEnv *env, jobject options, WebPAnimEncoderOptions *encoder_options) {
        if (options != nullptr) {
            jclass options_class = env->GetObjectClass(options);
            // minimizeSize
            jobject minimize_size = env->GetObjectField(options,
                                                        env->GetFieldID(options_class,
                                                                        "minimizeSize",
                                                                        "Ljava/lang/Boolean;"));
            if (minimize_size != nullptr) {
                encoder_options->minimize_size = GetBooleanValue(env, minimize_size);
            }
            // kmin
            jobject kmin = env->GetObjectField(options, env->GetFieldID(options_class, "kmin",
                                                                        "Ljava/lang/Integer;"));
            if (kmin != nullptr) {
                encoder_options->kmin = GetIntegerValue(env, kmin);
            }
            // kmax
            jobject kmax = env->GetObjectField(options, env->GetFieldID(options_class, "kmax",
                                                                        "Ljava/lang/Integer;"));
            if (kmax != nullptr) {
                encoder_options->kmax = GetIntegerValue(env, kmax);
            }
            // allowMixed
            jobject allow_mixed = env->GetObjectField(options,
                                                      env->GetFieldID(options_class, "allowMixed",
                                                                      "Ljava/lang/Boolean;"));
            if (allow_mixed != nullptr) {
                encoder_options->allow_mixed = GetBooleanValue(env, allow_mixed);
            }
            // verbose
            jobject verbose = env->GetObjectField(options, env->GetFieldID(options_class, "verbose",
                                                                           "Ljava/lang/Boolean;"));
            if (verbose != nullptr) {
                encoder_options->verbose = GetBooleanValue(env, verbose);
            }
            // anim params
            WebPMuxAnimParams anim_params;
            jobject anim_params_obj = env->GetObjectField(options, env->GetFieldID(options_class,
                                                                                   "animParams",
                                                                                   "Lcom/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams;"));
            if (anim_params_obj != nullptr) {
                jclass anim_params_class = env->GetObjectClass(anim_params_obj);
                // bgColor
                jobject background_color = env->GetObjectField(anim_params_obj,
                                                               env->GetFieldID(anim_params_class,
                                                                               "backgroundColor",
                                                                               "Ljava/lang/Integer;"));
                if (background_color != nullptr) {
                    anim_params.bgcolor = GetIntegerValue(env, background_color);
                } else {
                    anim_params.bgcolor = 0;
                }
                // loopCount
                jobject loop_count = env->GetObjectField(anim_params_obj,
                                                         env->GetFieldID(anim_params_class,
                                                                         "loopCount",
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
            jobject lossless = env->GetObjectField(config, env->GetFieldID(config_class, "lossless",
                                                                           "Ljava/lang/Integer;"));
            if (lossless != nullptr) {
                encoder_config->lossless = GetIntegerValue(env, lossless);
            }
            // quality
            jobject quality = env->GetObjectField(config, env->GetFieldID(config_class, "quality",
                                                                          "Ljava/lang/Float;"));
            if (quality != nullptr) {
                encoder_config->quality = GetFloatValue(env, quality);
            }
            // method
            jobject method = env->GetObjectField(config,
                                                 env->GetFieldID(config_class, "method",
                                                                 "Ljava/lang/Integer;"));
            if (method != nullptr) {
                encoder_config->target_size = GetIntegerValue(env, method);
            }
            // target_size
            jobject target_size = env->GetObjectField(config,
                                                      env->GetFieldID(config_class, "targetSize",
                                                                      "Ljava/lang/Integer;"));
            if (target_size != nullptr) {
                encoder_config->target_size = GetIntegerValue(env, target_size);
            }
            // target_PSNR
            jobject target_PSNR = env->GetObjectField(config,
                                                      env->GetFieldID(config_class, "targetPSNR",
                                                                      "Ljava/lang/Float;"));
            if (target_PSNR != nullptr) {
                encoder_config->target_PSNR = GetFloatValue(env, target_PSNR);
            }
            // segments
            jobject segments = env->GetObjectField(config,
                                                   env->GetFieldID(config_class, "segments",
                                                                   "Ljava/lang/Integer;"));
            if (segments != nullptr) {
                encoder_config->segments = GetIntegerValue(env, segments);
            }
            // snsStrength
            jobject sns_strength = env->GetObjectField(config,
                                                       env->GetFieldID(config_class, "snsStrength",
                                                                       "Ljava/lang/Integer;"));
            if (sns_strength != nullptr) {
                encoder_config->sns_strength = GetIntegerValue(env, sns_strength);
            }
            // filterStrength
            jobject filter_strength = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                  "filterStrength",
                                                                                  "Ljava/lang/Integer;"));
            if (filter_strength != nullptr) {
                encoder_config->filter_strength = GetIntegerValue(env, filter_strength);
            }
            // filterSharpness
            jobject filter_sharpness = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                   "filterSharpness",
                                                                                   "Ljava/lang/Integer;"));
            if (filter_sharpness != nullptr) {
                encoder_config->filter_sharpness = GetIntegerValue(env, filter_sharpness);
            }
            // filterType
            jobject filter_type = env->GetObjectField(config,
                                                      env->GetFieldID(config_class, "filterType",
                                                                      "Ljava/lang/Integer;"));
            if (filter_type != nullptr) {
                encoder_config->filter_type = GetIntegerValue(env, filter_type);
            }
            // autoFilter
            jobject auto_filter = env->GetObjectField(config,
                                                      env->GetFieldID(config_class, "autoFilter",
                                                                      "Ljava/lang/Boolean;"));
            if (auto_filter != nullptr) {
                encoder_config->autofilter = GetBooleanValue(env, auto_filter);
            }
            // alphaCompression
            jobject alpha_compression = env->GetObjectField(config,
                                                            env->GetFieldID(config_class,
                                                                            "alphaCompression",
                                                                            "Ljava/lang/Integer;"));
            if (alpha_compression != nullptr) {
                encoder_config->alpha_compression = GetIntegerValue(env, alpha_compression);
            }
            // alphaFiltering
            jobject alpha_filtering = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                  "alphaFiltering",
                                                                                  "Ljava/lang/Integer;"));
            if (alpha_filtering != nullptr) {
                encoder_config->alpha_filtering = GetIntegerValue(env, alpha_filtering);
            }

            // alphaQuality
            jobject alpha_quality = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                "alphaQuality",
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
            jobject show_compressed = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                  "showCompressed",
                                                                                  "Ljava/lang/Boolean;"));
            if (show_compressed != nullptr) {
                encoder_config->show_compressed = GetBooleanValue(env, show_compressed);
            }
            // preprocessing
            jobject preprocessing = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                "preprocessing",
                                                                                "Ljava/lang/Integer;"));
            if (preprocessing != nullptr) {
                encoder_config->preprocessing = GetIntegerValue(env, preprocessing);
            }
            // partitions
            jobject partitions = env->GetObjectField(config,
                                                     env->GetFieldID(config_class, "partitions",
                                                                     "Ljava/lang/Integer;"));
            if (preprocessing != nullptr) {
                encoder_config->partitions = GetIntegerValue(env, partitions);
            }
            // partitionLimit
            jobject partition_limit = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                  "partitionLimit",
                                                                                  "Ljava/lang/Integer;"));
            if (partition_limit != nullptr) {
                encoder_config->partition_limit = GetIntegerValue(env, partition_limit);
            }
            // emulateJPEGSize
            jobject emulate_jpeg_size = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                    "emulateJPEGSize",
                                                                                    "Ljava/lang/Boolean;"));
            if (emulate_jpeg_size != nullptr) {
                encoder_config->emulate_jpeg_size = GetBooleanValue(env, emulate_jpeg_size);
            }
            // threadLevel
            jobject thread_level = env->GetObjectField(config,
                                                       env->GetFieldID(config_class, "threadLevel",
                                                                       "Ljava/lang/Integer;"));
            if (thread_level != nullptr) {
                encoder_config->thread_level = GetIntegerValue(env, thread_level);
            }
            // lowMemory
            jobject low_memory = env->GetObjectField(config,
                                                     env->GetFieldID(config_class, "lowMemory",
                                                                     "Ljava/lang/Boolean;"));
            if (low_memory != nullptr) {
                encoder_config->low_memory = GetBooleanValue(env, low_memory);
            }
            // nearLossless
            jobject near_lossless = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                "nearLossless",
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
            jobject use_delta_palette = env->GetObjectField(config, env->GetFieldID(config_class,
                                                                                    "useDeltaPalette",
                                                                                    "Ljava/lang/Boolean;"));
            if (use_delta_palette != nullptr) {
                encoder_config->use_delta_palette = GetBooleanValue(env, use_delta_palette);
            }
            // useSharpYUV
            jobject use_sharp_yuv = env->GetObjectField(config,
                                                        env->GetFieldID(config_class, "useSharpYUV",
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
JNIEXPORT jlong
JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_create(JNIEnv *env, jobject
self,
                                                                       jint width, jint
                                                                       height,
                                                                       jobject options
) {
    env->GetJavaVM(&javaVm);
    WebPAnimEncoderOptions encoder_options;
    if (WebPAnimEncoderOptionsInit(&encoder_options)) {
        Encoder::ParseOptions(env, options, &encoder_options);
        auto *encoder = new Encoder(width, height, encoder_options, env->NewWeakGlobalRef(self));
        return reinterpret_cast<jlong>(encoder);
    } else {
        ThrowException(env, "WebPAnimEncoderOptionsInit failed");
    }
    return 0;
}

extern "C"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure__Lcom_aureusapps_android_webpandroid_encoder_WebPConfig_2(
        JNIEnv
        *env,
        jobject self,
        jobject
        config) {
    WebPConfig encoder_config;
    if (WebPConfigInit(&encoder_config)) {
        Encoder::ParseConfig(env, config, &encoder_config);
        if (WebPValidateConfig(&encoder_config)) {
            Encoder *encoder = Encoder::GetInstance(env, self);
            encoder->encoder_config = encoder_config;
        } else {
            ThrowException(env, "WebPValidateConfig failed");
        }
    } else {
        ThrowException(env, "WebPConfigInit failed");
    }
}
#pragma clang diagnostic pop

extern "C"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_configure__Lcom_aureusapps_android_webpandroid_encoder_WebPPreset_2F(
        JNIEnv
        *env,
        jobject self,
        jobject
        preset,
        jfloat quality
) {
    WebPPreset encoder_preset = Encoder::ParsePreset(env, preset);
    WebPConfig encoder_config;
    if (WebPConfigPreset(&encoder_config, encoder_preset, quality)) {
        Encoder *encoder = Encoder::GetInstance(env, self);
        encoder->encoder_config = encoder_config;
    } else {
        ThrowException(env, "WebPConfigPreset failed");
    }
}
#pragma clang diagnostic pop

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_addFrame(JNIEnv
                                                                         *env,
                                                                         jobject self, jobject
                                                                         frame) {
    jclass frame_class = env->GetObjectClass(frame);
    jobject bitmap = env->GetObjectField(frame,
                                         env->GetFieldID(frame_class, "bitmap",
                                                         "Landroid/graphics/Bitmap;"));
    jlong timestamp = env->GetLongField(frame, env->GetFieldID(frame_class, "timestamp", "J"));

    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        ThrowException(env, "AndroidBitmap_getInfo failed");
    }

    void *pixels;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        ThrowException(env, "AndroidBitmap_lockPixels failed");
    }

    WebPPicture webp_picture;
    if (WebPPictureInit(&webp_picture)) {
        webp_picture.use_argb = true;
        webp_picture.width = (int) info.width;
        webp_picture.height = (int) info.height;
        webp_picture.colorspace = WEBP_YUV420;
        webp_picture.argb_stride = 4;
        if (WebPPictureAlloc(&webp_picture)) {
            // copy pixel data from the bitmap
            uint32_t *dst = webp_picture.argb;
            for (int y = 0; y < info.height; ++y) {
                memcpy(dst, pixels, info.width * 4);
                pixels = (void *) ((uint8_t *) pixels + info.stride);
                dst += webp_picture.argb_stride;
            }
            AndroidBitmap_unlockPixels(env, bitmap);
            Encoder *encoder = Encoder::GetInstance(env, self);
            auto *user_data = new FrameUserData;
            user_data->encoder = encoder;
            user_data->frame_id = ++encoder->frame_count;
            webp_picture.user_data = user_data;
            webp_picture.progress_hook = Encoder::OnProgressUpdate;
            if (WebPAnimEncoderAdd(encoder->anim_encoder,
                                   &webp_picture, (int) timestamp,
                                   &encoder->encoder_config)) {
            } else {
                ThrowException(env, "WebPAnimEncoderAdd failed");
            }
        } else {
            ThrowException(env, "WebPPictureAlloc failed");
        }
    } else {
        ThrowException(env, "WebPPictureInit failed");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_assemble(JNIEnv
                                                                         *env,
                                                                         jobject self, jlong
                                                                         timestamp,
                                                                         jstring path
) {
    auto *encoder = Encoder::GetInstance(env, self);
    if (encoder != nullptr) {
        if (WebPAnimEncoderAdd(encoder->anim_encoder, nullptr, (int) timestamp, nullptr)) {
            WebPData data;
            WebPDataInit(&data);
            if (WebPAnimEncoderAssemble(encoder->anim_encoder, &data)) {
                const char *file_path = env->GetStringUTFChars(path, nullptr);
                FILE *file = fopen(file_path, "w+");
                if (file != nullptr) {
                    int ret = (int) fwrite(data.bytes, 1, data.size, file);
                    if (ret == data.size) {
                        fclose(file);
                    } else {
                        ThrowException(env, "File write failed");
                    }
                } else {
                    ThrowException(env, strerror(errno)
                    );
                }
            } else {
                ThrowException(env, "WebPAnimEncoderAssemble failed");
            }
        } else {
            ThrowException(env, "WebPAnimEncoderAdd failed");
        }
    } else {
        ThrowException(env, "Encoder is null");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_android_webpandroid_encoder_WebPAnimEncoder_release(JNIEnv
                                                                        *env,
                                                                        jobject self
) {
    auto *encoder = Encoder::GetInstance(env, self);
    if (encoder != nullptr) {
        WebPAnimEncoderDelete(encoder->anim_encoder);
        jclass encoder_class = env->GetObjectClass(self);
        env->SetLongField(self,
                          env->GetFieldID(encoder_class, "nativeObjectPointer", "J"),
                          (jlong) 0);
        env->DeleteWeakGlobalRef(encoder->java_encoder_ref);
        delete encoder;
    } else {
        ThrowException(env, "Encoder is null");
    }
}