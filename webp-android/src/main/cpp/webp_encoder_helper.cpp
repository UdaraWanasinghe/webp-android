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
    jobject lossless = type::getObjectField(env, jconfig, "lossless", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, lossless)) {
        config->lossless = type::getIntegerValue(env, lossless);
        env->DeleteLocalRef(lossless);
    }

    // quality
    jobject quality = type::getObjectField(env, jconfig, "quality", "Ljava/lang/Float;");
    if (!type::isObjectNull(env, quality)) {
        config->quality = type::getFloatValue(env, quality);
        env->DeleteLocalRef(quality);
    }

    // method
    jobject method = type::getObjectField(env, jconfig, "method", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, method)) {
        config->method = type::getIntegerValue(env, method);
        env->DeleteLocalRef(method);
    }

    // target size
    jobject target_size = type::getObjectField(env, jconfig, "targetSize", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, target_size)) {
        config->target_size = type::getIntegerValue(env, target_size);
        env->DeleteLocalRef(target_size);
    }

    // target PSNR
    jobject target_psnr = type::getObjectField(env, jconfig, "targetPSNR", "Ljava/lang/Float;");
    if (!type::isObjectNull(env, target_psnr)) {
        config->target_PSNR = type::getFloatValue(env, target_psnr);
        env->DeleteLocalRef(target_psnr);
    }

    // segments
    jobject segments = type::getObjectField(env, jconfig, "segments", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, segments)) {
        config->segments = type::getIntegerValue(env, segments);
        env->DeleteLocalRef(segments);
    }

    // sns_strength
    jobject sns_strength = type::getObjectField(env, jconfig, "snsStrength", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, sns_strength)) {
        config->sns_strength = type::getIntegerValue(env, sns_strength);
        env->DeleteLocalRef(sns_strength);
    }

    // filter_strength
    jobject filter_strength = type::getObjectField(
            env,
            jconfig,
            "filterStrength",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, filter_strength)) {
        config->filter_strength = type::getIntegerValue(env, filter_strength);
        env->DeleteLocalRef(filter_strength);
    }

    // filter_sharpness
    jobject filter_sharpness = type::getObjectField(
            env,
            jconfig,
            "filterSharpness",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, filter_sharpness)) {
        config->filter_sharpness = type::getIntegerValue(env, filter_sharpness);
        env->DeleteLocalRef(filter_sharpness);
    }

    // filter_type
    jobject filter_type = type::getObjectField(env, jconfig, "filterType", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, filter_type)) {
        config->filter_type = type::getIntegerValue(env, filter_type);
        env->DeleteLocalRef(filter_type);
    }

    // auto_filter
    jobject auto_filter = type::getObjectField(env, jconfig, "autoFilter", "Ljava/lang/Boolean;");
    if (!type::isObjectNull(env, auto_filter)) {
        config->autofilter = type::getBooleanValue(env, auto_filter);
        env->DeleteLocalRef(auto_filter);
    }

    // alpha_compression
    jobject alpha_compression = type::getObjectField(
            env,
            jconfig,
            "alphaCompression",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, alpha_compression)) {
        config->alpha_compression = type::getIntegerValue(env, alpha_compression);
        env->DeleteLocalRef(alpha_compression);
    }

    // alpha_filtering
    jobject alpha_filtering = type::getObjectField(
            env,
            jconfig,
            "alphaFiltering",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, alpha_filtering)) {
        config->alpha_filtering = type::getIntegerValue(env, alpha_filtering);
        env->DeleteLocalRef(alpha_filtering);
    }

    // alpha_quality
    jobject alpha_quality = type::getObjectField(
            env,
            jconfig,
            "alphaQuality",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, alpha_quality)) {
        config->alpha_quality = type::getIntegerValue(env, alpha_quality);
        env->DeleteLocalRef(alpha_quality);
    }

    // pass
    jobject pass = type::getObjectField(env, jconfig, "pass", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, pass)) {
        config->pass = type::getIntegerValue(env, pass);
        env->DeleteLocalRef(pass);
    }

    // show_compressed
    jobject show_compressed = type::getObjectField(
            env,
            jconfig,
            "showCompressed",
            "Ljava/lang/Boolean;"
    );
    if (!type::isObjectNull(env, show_compressed)) {
        config->show_compressed = type::getBooleanValue(env, show_compressed);
        env->DeleteLocalRef(show_compressed);
    }

    // preprocessing
    jobject preprocessing = type::getObjectField(
            env,
            jconfig,
            "preprocessing",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, preprocessing)) {
        config->preprocessing = type::getIntegerValue(env, preprocessing);
        env->DeleteLocalRef(preprocessing);
    }

    // partitions
    jobject partitions = type::getObjectField(env, jconfig, "partitions", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, partitions)) {
        config->partitions = type::getIntegerValue(env, partitions);
        env->DeleteLocalRef(partitions);
    }

    // partition_limit
    jobject partition_limit = type::getObjectField(
            env,
            jconfig,
            "partitionLimit",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, partition_limit)) {
        config->partition_limit = type::getIntegerValue(env, partition_limit);
        env->DeleteLocalRef(partition_limit);
    }

    // emulate_jpeg_size
    jobject emulate_jpeg_size = type::getObjectField(
            env,
            jconfig,
            "emulateJPEGSize",
            "Ljava/lang/Boolean;"
    );
    if (!type::isObjectNull(env, emulate_jpeg_size)) {
        config->emulate_jpeg_size = type::getBooleanValue(env, emulate_jpeg_size);
        env->DeleteLocalRef(emulate_jpeg_size);
    }

    // thread_level
    jobject thread_level = type::getObjectField(env, jconfig, "threadLevel", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, thread_level)) {
        config->thread_level = type::getIntegerValue(env, thread_level);
        env->DeleteLocalRef(thread_level);
    }

    // low_memory
    jobject low_memory = type::getObjectField(env, jconfig, "lowMemory", "Ljava/lang/Boolean;");
    if (!type::isObjectNull(env, low_memory)) {
        config->low_memory = type::getBooleanValue(env, low_memory);
        env->DeleteLocalRef(low_memory);
    }

    // near_lossless
    jobject near_lossless = type::getObjectField(
            env,
            jconfig,
            "nearLossless",
            "Ljava/lang/Integer;"
    );
    if (!type::isObjectNull(env, near_lossless)) {
        config->near_lossless = type::getIntegerValue(env, near_lossless);
        env->DeleteLocalRef(near_lossless);
    }

    // exact
    jobject exact = type::getObjectField(env, jconfig, "exact", "Ljava/lang/Boolean;");
    if (!type::isObjectNull(env, exact)) {
        config->exact = type::getBooleanValue(env, exact);
        env->DeleteLocalRef(exact);
    }

    // use_delta_palette
    jobject use_delta_palette = type::getObjectField(
            env,
            jconfig,
            "useDeltaPalette",
            "Ljava/lang/Boolean;"
    );
    if (!type::isObjectNull(env, use_delta_palette)) {
        config->use_delta_palette = type::getBooleanValue(env, use_delta_palette);
        env->DeleteLocalRef(use_delta_palette);
    }

    // use_sharp_yuv
    jobject use_sharp_yuv = type::getObjectField(env, jconfig, "useSharpYUV",
                                                 "Ljava/lang/Boolean;");
    if (!type::isObjectNull(env, use_sharp_yuv)) {
        config->use_sharp_yuv = type::getBooleanValue(env, use_sharp_yuv);
        env->DeleteLocalRef(use_sharp_yuv);
    }

    // qmin
    jobject qmin = type::getObjectField(env, jconfig, "qmin", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, qmin)) {
        config->qmin = type::getIntegerValue(env, qmin);
        env->DeleteLocalRef(qmin);
    }

    // qmax
    jobject qmax = type::getObjectField(env, jconfig, "qmax", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, qmax)) {
        config->qmax = type::getIntegerValue(env, qmax);
        env->DeleteLocalRef(qmax);
    }

}

float parseWebPQuality(JNIEnv *env, jobject jconfig) {
    jclass config_class = env->GetObjectClass(jconfig);
    jfieldID quality_field_id = env->GetFieldID(config_class, "quality", "Ljava/lang/Float;");
    jobject quality_field = env->GetObjectField(jconfig, quality_field_id);
    float quality;
    if (type::isObjectNull(env, quality_field)) {
        quality = 70.0f;

    } else {
        quality = type::getFloatValue(env, quality_field);
        env->DeleteLocalRef(quality_field);

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
    jobject minimize_size = type::getObjectField(
            env,
            joptions,
            "minimizeSize",
            "Ljava/lang/Boolean;"
    );
    if (!type::isObjectNull(env, minimize_size)) {
        options->minimize_size = type::getBooleanValue(env, minimize_size);
        env->DeleteLocalRef(minimize_size);
    }

    // kmin
    jobject kmin = type::getObjectField(env, joptions, "kmin", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, kmin)) {
        options->kmin = type::getIntegerValue(env, kmin);
        env->DeleteLocalRef(kmin);
    }

    // kmax
    jobject kmax = type::getObjectField(env, joptions, "kmax", "Ljava/lang/Integer;");
    if (!type::isObjectNull(env, kmax)) {
        options->kmax = type::getIntegerValue(env, kmax);
        env->DeleteLocalRef(kmax);
    }

    // allowMixed
    jobject allow_mixed = type::getObjectField(env, joptions, "allowMixed", "Ljava/lang/Boolean;");
    if (!type::isObjectNull(env, allow_mixed)) {
        options->allow_mixed = type::getBooleanValue(env, allow_mixed);
        env->DeleteLocalRef(allow_mixed);
    }

    // verbose
    jobject verbose = type::getObjectField(env, joptions, "verbose", "Ljava/lang/Boolean;");
    if (!type::isObjectNull(env, verbose)) {
        options->verbose = type::getBooleanValue(env, verbose);
        env->DeleteLocalRef(verbose);
    }

    // anim params
    WebPMuxAnimParams params;
    jobject jparams = type::getObjectField(
            env,
            joptions,
            "animParams",
            "Lcom/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams;"
    );
    if (!type::isObjectNull(env, jparams)) {
        // bgColor
        jobject bg_color = type::getObjectField(
                env,
                jparams,
                "backgroundColor",
                "Ljava/lang/Integer;"
        );
        if (!type::isObjectNull(env, bg_color)) {
            params.bgcolor = type::getIntegerValue(env, bg_color);
            env->DeleteLocalRef(bg_color);
        } else {
            params.bgcolor = 0;
        }

        // loop_count
        jobject loop_count = type::getObjectField(env, jparams, "loopCount", "Ljava/lang/Integer;");
        if (!type::isObjectNull(env, loop_count)) {
            params.loop_count = type::getIntegerValue(env, loop_count);
            env->DeleteLocalRef(loop_count);
        } else {
            params.loop_count = 1;
        }
        env->DeleteLocalRef(jparams);

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