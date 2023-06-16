//
// Created by udara on 6/5/23.
//

#include "include/webp_encoder_helper.h"
#include "include/exception_helper.h"
#include "include/type_helper.h"

WebPPreset parseWebPPreset(JNIEnv *env, jobject jpreset) {
    // get preset class
    jclass preset_class = env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPPreset");

    // check instance
    if (!env->IsInstanceOf(jpreset, preset_class)) {
        throw std::runtime_error("Given preset object is not of type WebPPreset");
    }

    // get ordinal
    jfieldID ordinal_field_id = env->GetFieldID(preset_class, "value", "I");
    jint ordinal = env->GetIntField(jpreset, ordinal_field_id);

    // Delete local ref
    env->DeleteLocalRef(preset_class);

    return WebPPreset(ordinal);
}

void applyWebPConfig(JNIEnv *env, jobject jconfig, WebPConfig *config) {
    // lossless
    jobject lossless = getObjectField(env, jconfig, "lossless", "Ljava/lang/Integer;");
    if (lossless != nullptr) {
        config->lossless = getIntegerValue(env, lossless);
    }

    // quality
    jobject quality = getObjectField(env, jconfig, "quality", "Ljava/lang/Float;");
    if (quality != nullptr) {
        config->quality = getFloatValue(env, quality);
    }

    // method
    jobject method = getObjectField(env, jconfig, "method", "Ljava/lang/Integer;");
    if (method != nullptr) {
        config->method = getIntegerValue(env, method);
    }

    // target size
    jobject target_size = getObjectField(env, jconfig, "targetSize", "Ljava/lang/Integer;");
    if (target_size != nullptr) {
        config->target_size = getIntegerValue(env, target_size);
    }

    // target PSNR
    jobject target_psnr = getObjectField(env, jconfig, "targetPSNR", "Ljava/lang/Float;");
    if (target_psnr != nullptr) {
        config->target_PSNR = getFloatValue(env, target_psnr);
    }

    // segments
    jobject segments = getObjectField(env, jconfig, "segments", "Ljava/lang/Integer;");
    if (segments != nullptr) {
        config->segments = getIntegerValue(env, segments);
    }

    // sns_strength
    jobject sns_strength = getObjectField(env, jconfig, "snsStrength", "Ljava/lang/Integer;");
    if (sns_strength != nullptr) {
        config->sns_strength = getIntegerValue(env, sns_strength);
    }

    // filter_strength
    jobject filter_strength = getObjectField(
            env,
            jconfig,
            "filterStrength",
            "Ljava/lang/Integer;"
    );
    if (filter_strength != nullptr) {
        config->filter_strength = getIntegerValue(env, filter_strength);
    }

    // filter_sharpness
    jobject filter_sharpness = getObjectField(
            env,
            jconfig,
            "filterSharpness",
            "Ljava/lang/Integer;"
    );
    if (filter_sharpness != nullptr) {
        config->filter_sharpness = getIntegerValue(env, filter_sharpness);
    }

    // filter_type
    jobject filter_type = getObjectField(env, jconfig, "filterType", "Ljava/lang/Integer;");
    if (filter_type != nullptr) {
        config->filter_type = getIntegerValue(env, filter_type);
    }

    // auto_filter
    jobject auto_filter = getObjectField(env, jconfig, "autoFilter", "Ljava/lang/Boolean;");
    if (auto_filter != nullptr) {
        config->autofilter = getBooleanValue(env, auto_filter);
    }

    // alpha_compression
    jobject alpha_compression = getObjectField(
            env,
            jconfig,
            "alphaCompression",
            "Ljava/lang/Integer;"
    );
    if (alpha_compression != nullptr) {
        config->alpha_compression = getIntegerValue(env, alpha_compression);
    }

    // alpha_filtering
    jobject alpha_filtering = getObjectField(
            env,
            jconfig,
            "alphaFiltering",
            "Ljava/lang/Integer;"
    );
    if (alpha_filtering != nullptr) {
        config->alpha_filtering = getIntegerValue(env, alpha_filtering);
    }

    // alpha_quality
    jobject alpha_quality = getObjectField(
            env,
            jconfig,
            "alphaQuality",
            "Ljava/lang/Integer;"
    );
    if (alpha_quality != nullptr) {
        config->alpha_quality = getIntegerValue(env, alpha_quality);
    }

    // pass
    jobject pass = getObjectField(env, jconfig, "pass", "Ljava/lang/Integer;");
    if (pass != nullptr) {
        config->pass = getIntegerValue(env, pass);
    }

    // show_compressed
    jobject show_compressed = getObjectField(
            env,
            jconfig,
            "showCompressed",
            "Ljava/lang/Boolean;"
    );
    if (show_compressed != nullptr) {
        config->show_compressed = getBooleanValue(env, show_compressed);
    }

    // preprocessing
    jobject preprocessing = getObjectField(
            env,
            jconfig,
            "preprocessing",
            "Ljava/lang/Integer;"
    );
    if (preprocessing != nullptr) {
        config->preprocessing = getIntegerValue(env, preprocessing);
    }

    // partitions
    jobject partitions = getObjectField(env, jconfig, "partitions", "Ljava/lang/Integer;");
    if (partitions != nullptr) {
        config->partitions = getIntegerValue(env, partitions);
    }

    // partition_limit
    jobject partition_limit = getObjectField(
            env,
            jconfig,
            "partitionLimit",
            "Ljava/lang/Integer;"
    );
    if (partition_limit != nullptr) {
        config->partition_limit = getIntegerValue(env, partition_limit);
    }

    // emulate_jpeg_size
    jobject emulate_jpeg_size = getObjectField(
            env,
            jconfig,
            "emulateJPEGSize",
            "Ljava/lang/Boolean;"
    );
    if (emulate_jpeg_size != nullptr) {
        config->emulate_jpeg_size = getBooleanValue(env, emulate_jpeg_size);
    }

    // thread_level
    jobject thread_level = getObjectField(env, jconfig, "threadLevel", "Ljava/lang/Integer;");
    if (thread_level != nullptr) {
        config->thread_level = getIntegerValue(env, thread_level);
    }

    // low_memory
    jobject low_memory = getObjectField(env, jconfig, "lowMemory", "Ljava/lang/Boolean;");
    if (low_memory != nullptr) {
        config->low_memory = getBooleanValue(env, low_memory);
    }

    // near_lossless
    jobject near_lossless = getObjectField(
            env,
            jconfig,
            "nearLossless",
            "Ljava/lang/Integer;"
    );
    if (near_lossless != nullptr) {
        config->near_lossless = getIntegerValue(env, near_lossless);
    }

    // exact
    jobject exact = getObjectField(env, jconfig, "exact", "Ljava/lang/Boolean;");
    if (exact != nullptr) {
        config->exact = getBooleanValue(env, exact);
    }

    // use_delta_palette
    jobject use_delta_palette = getObjectField(
            env,
            jconfig,
            "useDeltaPalette",
            "Ljava/lang/Boolean;"
    );
    if (use_delta_palette != nullptr) {
        config->use_delta_palette = getBooleanValue(env, use_delta_palette);
    }

    // use_sharp_yuv
    jobject use_sharp_yuv = getObjectField(env, jconfig, "useSharpYUV", "Ljava/lang/Boolean;");
    if (use_sharp_yuv != nullptr) {
        config->use_sharp_yuv = getBooleanValue(env, use_sharp_yuv);
    }

    // qmin
    jobject qmin = getObjectField(env, jconfig, "qmin", "Ljava/lang/Integer;");
    if (qmin != nullptr) {
        config->qmin = getIntegerValue(env, qmin);
    }

    // qmax
    jobject qmax = getObjectField(env, jconfig, "qmax", "Ljava/lang/Integer;");
    if (qmax != nullptr) {
        config->qmax = getIntegerValue(env, qmax);
    }

}

float parseWebPQuality(JNIEnv *env, jobject jconfig) {
    jclass config_class = env->GetObjectClass(jconfig);
    jfieldID quality_field_id = env->GetFieldID(config_class, "quality", "Ljava/lang/Float;");
    jobject quality_field = env->GetObjectField(jconfig, quality_field_id);
    float quality;
    if (quality_field == nullptr) {
        quality = 70.0f;
    } else {
        quality = getFloatValue(env, quality_field);
    }
    env->DeleteLocalRef(config_class);
    return quality;
}

void parseEncoderOptions(
        JNIEnv *env,
        jobject joptions,
        WebPAnimEncoderOptions *options
) {
    // minimize_size
    jobject minimize_size = getObjectField(env, joptions, "minimizeSize", "Ljava/lang/Boolean;");
    if (minimize_size != nullptr) {
        options->minimize_size = getBooleanValue(env, minimize_size);
    }

    // kmin
    jobject kmin = getObjectField(env, joptions, "kmin", "Ljava/lang/Integer;");
    if (kmin != nullptr) {
        options->kmin = getIntegerValue(env, kmin);
    }

    // kmax
    jobject kmax = getObjectField(env, joptions, "kmax", "Ljava/lang/Integer;");
    if (kmax != nullptr) {
        options->kmax = getIntegerValue(env, kmax);
    }

    // allowMixed
    jobject allow_mixed = getObjectField(env, joptions, "allowMixed", "Ljava/lang/Boolean;");
    if (allow_mixed != nullptr) {
        options->allow_mixed = getBooleanValue(env, allow_mixed);
    }

    // verbose
    jobject verbose = getObjectField(env, joptions, "verbose", "Ljava/lang/Boolean;");
    if (verbose != nullptr) {
        options->verbose = getBooleanValue(env, verbose);
    }

    // anim params
    WebPMuxAnimParams params;
    jobject jparams = getObjectField(
            env,
            joptions,
            "animParams",
            "Lcom/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams;"
    );
    if (jparams != nullptr) {
        // bgColor
        jobject bg_color = getObjectField(
                env,
                jparams,
                "backgroundColor",
                "Ljava/lang/Integer;"
        );
        if (bg_color != nullptr) {
            params.bgcolor = getIntegerValue(env, bg_color);
        } else {
            params.bgcolor = 0;
        }

        // loop_count
        jobject loop_count = getObjectField(env, jparams, "loopCount", "Ljava/lang/Integer;");
        if (loop_count != nullptr) {
            params.loop_count = getIntegerValue(env, loop_count);
        } else {
            params.loop_count = 1;
        }

    } else {
        params.bgcolor = 0;
        params.loop_count = 1;
    }
    options->anim_params = params;
}

void copyPixels(
        const uint8_t *src,
        WebPPicture *pic
) {
    // Need to swap rb channels
    // Android bitmap -> int color = (A & 0xff) << 24 | (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff)
    // Encoder pixels -> int color = (A & 0xFF) << 24 | (R & 0xFF) << 16 | (G & 0xFF) << 8 | (B & 0xFF)
    auto *dst = reinterpret_cast<uint8_t *>(pic->argb);
    const uint8_t *end = src + pic->width * pic->height * 4;
    while (src < end) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        *dst++ = src[0];
        *dst++ = src[3];
        *dst++ = src[2];
        *dst++ = src[1];
        src += 4;
#else
        *dst++ = src[2];
        *dst++ = src[1];
        *dst++ = src[0];
        *dst++ = src[3];
        src += 4;
#endif
    }
}