//
// Created by udara on 6/5/23.
//

#include "include/webp_encoder_helper.h"
#include "include/exception_helper.h"
#include "include/type_helper.h"

WebPPreset parseWebPPreset(JNIEnv *env, jobject *jpreset) {
    // get preset class
    jclass preset_class = env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPPreset");

    // check instance
    if (!env->IsInstanceOf(*jpreset, preset_class)) {
        throw std::runtime_error("Given jpreset is not of type WebPPreset");
    }

    // get ordinal
    jfieldID ordinal_field_id = env->GetFieldID(preset_class, "value", "I");
    jint ordinal = env->GetIntField(*jpreset, ordinal_field_id);

    // Delete local ref
    env->DeleteLocalRef(preset_class);

    return WebPPreset(ordinal);
}

void parseWebPConfig(JNIEnv *env, jobject *jconfig, WebPConfig *config) {
    // lossless
    jobject lossless = getObjectField(env, jconfig, "lossless", "Ljava/lang/Integer;");
    if (lossless != nullptr) {
        config->lossless = getIntegerValue(env, &lossless);
    }

    // quality
    jobject quality = getObjectField(env, jconfig, "quality", "Ljava/lang/Float;");
    if (quality != nullptr) {
        config->quality = getFloatValue(env, &quality);
    }

    // method
    jobject method = getObjectField(env, jconfig, "method", "Ljava/lang/Integer;");
    if (method != nullptr) {
        config->method = getIntegerValue(env, &method);
    }

    // target size
    jobject targetSize = getObjectField(env, jconfig, "targetSize", "Ljava/lang/Integer;");
    if (targetSize != nullptr) {
        config->target_size = getIntegerValue(env, &targetSize);
    }

    // target PSNR
    jobject targetPSNR = getObjectField(env, jconfig, "targetPSNR", "Ljava/lang/Float;");
    if (targetPSNR != nullptr) {
        config->target_PSNR = getFloatValue(env, &targetPSNR);
    }

    // segments
    jobject segments = getObjectField(env, jconfig, "segments", "Ljava/lang/Integer;");
    if (segments != nullptr) {
        config->segments = getIntegerValue(env, &segments);
    }

    // snsStrength
    jobject snsStrength = getObjectField(env, jconfig, "snsStrength", "Ljava/lang/Integer;");
    if (snsStrength != nullptr) {
        config->sns_strength = getIntegerValue(env, &snsStrength);
    }

    // filterStrength
    jobject filterStrength = getObjectField(
            env,
            jconfig,
            "filterStrength",
            "Ljava/lang/Integer;"
    );
    if (filterStrength != nullptr) {
        config->filter_strength = getIntegerValue(env, &filterStrength);
    }

    // filterSharpness
    jobject filterSharpness = getObjectField(
            env,
            jconfig,
            "filterSharpness",
            "Ljava/lang/Integer;"
    );
    if (filterSharpness != nullptr) {
        config->filter_sharpness = getIntegerValue(env, &filterSharpness);
    }

    // filterType
    jobject filterType = getObjectField(env, jconfig, "filterType", "Ljava/lang/Integer;");
    if (filterType != nullptr) {
        config->filter_type = getIntegerValue(env, &filterType);
    }

    // autoFilter
    jobject autoFilter = getObjectField(env, jconfig, "autoFilter", "Ljava/lang/Boolean;");
    if (autoFilter != nullptr) {
        config->autofilter = getBooleanValue(env, &autoFilter);
    }

    // alphaCompression
    jobject alphaCompression = getObjectField(
            env,
            jconfig,
            "alphaCompression",
            "Ljava/lang/Integer;"
    );
    if (alphaCompression != nullptr) {
        config->alpha_compression = getIntegerValue(env, &alphaCompression);
    }

    // alphaFiltering
    jobject alphaFiltering = getObjectField(
            env,
            jconfig,
            "alphaFiltering",
            "Ljava/lang/Integer;"
    );
    if (alphaFiltering != nullptr) {
        config->alpha_filtering = getIntegerValue(env, &alphaFiltering);
    }

    // alphaQuality
    jobject alphaQuality = getObjectField(
            env,
            jconfig,
            "alphaQuality",
            "Ljava/lang/Integer;"
    );
    if (alphaQuality != nullptr) {
        config->alpha_quality = getIntegerValue(env, &alphaQuality);
    }

    // pass
    jobject pass = getObjectField(env, jconfig, "pass", "Ljava/lang/Integer;");
    if (pass != nullptr) {
        config->pass = getIntegerValue(env, &pass);
    }

    // showCompressed
    jobject showCompressed = getObjectField(
            env,
            jconfig,
            "showCompressed",
            "Ljava/lang/Boolean;"
    );
    if (showCompressed != nullptr) {
        config->show_compressed = getBooleanValue(env, &showCompressed);
    }

    // preprocessing
    jobject preprocessing = getObjectField(
            env,
            jconfig,
            "preprocessing",
            "Ljava/lang/Integer;"
    );
    if (preprocessing != nullptr) {
        config->preprocessing = getIntegerValue(env, &preprocessing);
    }

    // partitions
    jobject partitions = getObjectField(env, jconfig, "partitions", "Ljava/lang/Integer;");
    if (partitions != nullptr) {
        config->partitions = getIntegerValue(env, &partitions);
    }

    // partitionLimit
    jobject partitionLimit = getObjectField(
            env,
            jconfig,
            "partitionLimit",
            "Ljava/lang/Integer;"
    );
    if (partitionLimit != nullptr) {
        config->partition_limit = getIntegerValue(env, &partitionLimit);
    }

    // emulateJPEGSize
    jobject emulateJPEGSize = getObjectField(
            env,
            jconfig,
            "emulateJPEGSize",
            "Ljava/lang/Boolean;"
    );
    if (emulateJPEGSize != nullptr) {
        config->emulate_jpeg_size = getBooleanValue(env, &emulateJPEGSize);
    }

    // threadLevel
    jobject threadLevel = getObjectField(env, jconfig, "threadLevel", "Ljava/lang/Integer;");
    if (threadLevel != nullptr) {
        config->thread_level = getIntegerValue(env, &threadLevel);
    }

    // lowMemory
    jobject lowMemory = getObjectField(env, jconfig, "lowMemory", "Ljava/lang/Boolean;");
    if (lowMemory != nullptr) {
        config->low_memory = getBooleanValue(env, &lowMemory);
    }

    // nearLossless
    jobject nearLossless = getObjectField(
            env,
            jconfig,
            "nearLossless",
            "Ljava/lang/Integer;"
    );
    if (nearLossless != nullptr) {
        config->near_lossless = getIntegerValue(env, &nearLossless);
    }

    // exact
    jobject exact = getObjectField(env, jconfig, "exact", "Ljava/lang/Boolean;");
    if (exact != nullptr) {
        config->exact = getBooleanValue(env, &exact);
    }

    // useDeltaPalette
    jobject useDeltaPalette = getObjectField(
            env,
            jconfig,
            "useDeltaPalette",
            "Ljava/lang/Boolean;"
    );
    if (useDeltaPalette != nullptr) {
        config->use_delta_palette = getBooleanValue(env, &useDeltaPalette);
    }

    // useSharpYUV
    jobject useSharpYUV = getObjectField(env, jconfig, "useSharpYUV", "Ljava/lang/Boolean;");
    if (useSharpYUV != nullptr) {
        config->use_sharp_yuv = getBooleanValue(env, &useSharpYUV);
    }

    // qmin
    jobject qmin = getObjectField(env, jconfig, "qmin", "Ljava/lang/Integer;");
    if (qmin != nullptr) {
        config->qmin = getIntegerValue(env, &qmin);
    }

    // qmax
    jobject qmax = getObjectField(env, jconfig, "qmax", "Ljava/lang/Integer;");
    if (qmax != nullptr) {
        config->qmax = getIntegerValue(env, &qmax);
    }

}

float parseWebPQuality(JNIEnv *env, jobject *jconfig) {
    jclass config_class = env->GetObjectClass(*jconfig);
    jfieldID quality_field_id = env->GetFieldID(config_class, "quality", "Ljava/lang/Float;");
    jobject quality_field = env->GetObjectField(*jconfig, quality_field_id);
    float quality;
    if (quality_field == nullptr) {
        quality = 70.0f;
    } else {
        quality = getFloatValue(env, &quality_field);
    }
    env->DeleteLocalRef(config_class);
    return quality;
}

void parseEncoderOptions(JNIEnv *env, jobject *joptions, WebPAnimEncoderOptions *options) {
    // minimizeSize
    jobject minimizeSize = getObjectField(env, joptions, "minimizeSize", "Ljava/lang/Boolean;");
    if (minimizeSize != nullptr) {
        options->minimize_size = getBooleanValue(env, &minimizeSize);
    }

    // kmin
    jobject kmin = getObjectField(env, joptions, "kmin", "Ljava/lang/Integer;");
    if (kmin != nullptr) {
        options->kmin = getIntegerValue(env, &kmin);
    }

    // kmax
    jobject kmax = getObjectField(env, joptions, "kmax", "Ljava/lang/Integer;");
    if (kmax != nullptr) {
        options->kmax = getIntegerValue(env, &kmax);
    }

    // allowMixed
    jobject allow_mixed = getObjectField(env, joptions, "allowMixed", "Ljava/lang/Boolean;");
    if (allow_mixed != nullptr) {
        options->allow_mixed = getBooleanValue(env, &allow_mixed);
    }

    // verbose
    jobject verbose = getObjectField(env, joptions, "verbose", "Ljava/lang/Boolean;");
    if (verbose != nullptr) {
        options->verbose = getBooleanValue(env, &verbose);
    }

    // anim params
    WebPMuxAnimParams webPMuxAnimParams;
    jobject animParams = getObjectField(
            env,
            joptions,
            "animParams",
            "Lcom/aureusapps/android/webpandroid/encoder/WebPMuxAnimParams;"
    );
    if (animParams != nullptr) {
        // bgColor
        jobject backgroundColor = getObjectField(
                env,
                &animParams,
                "backgroundColor",
                "Ljava/lang/Integer;"
        );
        if (backgroundColor != nullptr) {
            webPMuxAnimParams.bgcolor = getIntegerValue(env, &backgroundColor);
        } else {
            webPMuxAnimParams.bgcolor = 0;
        }

        // loopCount
        jobject loopCount = getObjectField(env, &animParams, "loopCount", "Ljava/lang/Integer;");
        if (loopCount != nullptr) {
            webPMuxAnimParams.loop_count = getIntegerValue(env, &loopCount);
        } else {
            webPMuxAnimParams.loop_count = 1;
        }

    } else {
        webPMuxAnimParams.bgcolor = 0;
        webPMuxAnimParams.loop_count = 1;
    }
    options->anim_params = webPMuxAnimParams;
}

void copyPixels(uint8_t *src, WebPPicture *pic) {
    // Need to swap rb channels
    // Android bitmap -> int color = (A & 0xff) << 24 | (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff)
    // Encoder pixels -> int color = (A & 0xFF) << 24 | (R & 0xFF) << 16 | (G & 0xFF) << 8 | (B & 0xFF)
    auto *dst = reinterpret_cast<uint8_t *>(pic->argb);
    uint8_t *end = src + pic->width * pic->height * 4;
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