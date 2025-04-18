//
// Created by udara on 6/5/23.
//

#include <stdexcept>

#include "include/encoder_helper.h"
#include "include/type_helper.h"
#include "include/native_loader.h"

WebPPreset enc::parseWebPPreset(JNIEnv *env, jobject jpreset) {
    // check instance
    if (!env->IsInstanceOf(jpreset, ClassRegistry::webPPresetClass.get(env))) {
        throw std::runtime_error("Given preset object is not of type WebPPreset");
    }

    // get ordinal
    jint ordinal = env->GetIntField(jpreset, ClassRegistry::webPPresetOrdinalFieldID.get(env));

    return WebPPreset(ordinal);
}

void enc::applyWebPConfig(JNIEnv *env, jobject jconfig, WebPConfig *config) {
    // lossless
    jobject lossless = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigLosslessFieldID.get(env)
    );
    if (!type::isObjectNull(env, lossless)) {
        config->lossless = type::getIntegerValue(env, lossless);
        env->DeleteLocalRef(lossless);
    }

    // quality
    jobject quality = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigQualityFieldID.get(env)
    );
    if (!type::isObjectNull(env, quality)) {
        config->quality = type::getFloatValue(env, quality);
        env->DeleteLocalRef(quality);
    }

    // method
    jobject method = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigMethodFieldID.get(env)
    );
    if (!type::isObjectNull(env, method)) {
        config->method = type::getIntegerValue(env, method);
        env->DeleteLocalRef(method);
    }

    // target size
    jobject target_size = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigTargetSizeFieldID.get(env)
    );
    if (!type::isObjectNull(env, target_size)) {
        config->target_size = type::getIntegerValue(env, target_size);
        env->DeleteLocalRef(target_size);
    }

    // target PSNR
    jobject target_psnr = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigTargetPSNRFieldID.get(env)
    );
    if (!type::isObjectNull(env, target_psnr)) {
        config->target_PSNR = type::getFloatValue(env, target_psnr);
        env->DeleteLocalRef(target_psnr);
    }

    // segments
    jobject segments = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigSegmentsFieldID.get(env)
    );
    if (!type::isObjectNull(env, segments)) {
        config->segments = type::getIntegerValue(env, segments);
        env->DeleteLocalRef(segments);
    }

    // sns_strength
    jobject sns_strength = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigSnsStrengthFieldID.get(env)
    );
    if (!type::isObjectNull(env, sns_strength)) {
        config->sns_strength = type::getIntegerValue(env, sns_strength);
        env->DeleteLocalRef(sns_strength);
    }

    // filter_strength
    jobject filter_strength = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigFilterStrengthFieldID.get(env)
    );
    if (!type::isObjectNull(env, filter_strength)) {
        config->filter_strength = type::getIntegerValue(env, filter_strength);
        env->DeleteLocalRef(filter_strength);
    }

    // filter_sharpness
    jobject filter_sharpness = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigFilterSharpnessFieldID.get(env)
    );
    if (!type::isObjectNull(env, filter_sharpness)) {
        config->filter_sharpness = type::getIntegerValue(env, filter_sharpness);
        env->DeleteLocalRef(filter_sharpness);
    }

    // filter_type
    jobject filter_type = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigFilterTypeFieldID.get(env)
    );
    if (!type::isObjectNull(env, filter_type)) {
        config->filter_type = type::getIntegerValue(env, filter_type);
        env->DeleteLocalRef(filter_type);
    }

    // auto_filter
    jobject auto_filter = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigAutoFilterFieldID.get(env)
    );
    if (!type::isObjectNull(env, auto_filter)) {
        config->autofilter = type::getBooleanValue(env, auto_filter);
        env->DeleteLocalRef(auto_filter);
    }

    // alpha_compression
    jobject alpha_compression = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigAlphaCompressionFieldID.get(env)
    );
    if (!type::isObjectNull(env, alpha_compression)) {
        config->alpha_compression = type::getIntegerValue(env, alpha_compression);
        env->DeleteLocalRef(alpha_compression);
    }

    // alpha_filtering
    jobject alpha_filtering = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigAlphaFilteringFieldID.get(env)
    );
    if (!type::isObjectNull(env, alpha_filtering)) {
        config->alpha_filtering = type::getIntegerValue(env, alpha_filtering);
        env->DeleteLocalRef(alpha_filtering);
    }

    // alpha_quality
    jobject alpha_quality = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigAlphaQualityFieldID.get(env)
    );
    if (!type::isObjectNull(env, alpha_quality)) {
        config->alpha_quality = type::getIntegerValue(env, alpha_quality);
        env->DeleteLocalRef(alpha_quality);
    }

    // pass
    jobject pass = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigPassFieldID.get(env)
    );
    if (!type::isObjectNull(env, pass)) {
        config->pass = type::getIntegerValue(env, pass);
        env->DeleteLocalRef(pass);
    }

    // show_compressed
    jobject show_compressed = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigShowCompressedFieldID.get(env)
    );
    if (!type::isObjectNull(env, show_compressed)) {
        config->show_compressed = type::getBooleanValue(env, show_compressed);
        env->DeleteLocalRef(show_compressed);
    }

    // preprocessing
    jobject preprocessing = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigPreprocessingFieldID.get(env)
    );
    if (!type::isObjectNull(env, preprocessing)) {
        config->preprocessing = type::getIntegerValue(env, preprocessing);
        env->DeleteLocalRef(preprocessing);
    }

    // partitions
    jobject partitions = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigPartitionsFieldID.get(env)
    );
    if (!type::isObjectNull(env, partitions)) {
        config->partitions = type::getIntegerValue(env, partitions);
        env->DeleteLocalRef(partitions);
    }

    // partition_limit
    jobject partition_limit = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigPartitionLimitFieldID.get(env)
    );
    if (!type::isObjectNull(env, partition_limit)) {
        config->partition_limit = type::getIntegerValue(env, partition_limit);
        env->DeleteLocalRef(partition_limit);
    }

    // emulate_jpeg_size
    jobject emulate_jpeg_size = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigEmulateJPEGSizeFieldID.get(env)
    );
    if (!type::isObjectNull(env, emulate_jpeg_size)) {
        config->emulate_jpeg_size = type::getBooleanValue(env, emulate_jpeg_size);
        env->DeleteLocalRef(emulate_jpeg_size);
    }

    // thread_level
    jobject thread_level = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigThreadLevelFieldID.get(env)
    );
    if (!type::isObjectNull(env, thread_level)) {
        config->thread_level = type::getIntegerValue(env, thread_level);
        env->DeleteLocalRef(thread_level);
    }

    // low_memory
    jobject low_memory = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigLowMemoryFieldID.get(env)
    );
    if (!type::isObjectNull(env, low_memory)) {
        config->low_memory = type::getBooleanValue(env, low_memory);
        env->DeleteLocalRef(low_memory);
    }

    // near_lossless
    jobject near_lossless = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigNearLosslessFieldID.get(env)
    );
    if (!type::isObjectNull(env, near_lossless)) {
        config->near_lossless = type::getIntegerValue(env, near_lossless);
        env->DeleteLocalRef(near_lossless);
    }

    // exact
    jobject exact = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigExactFieldID.get(env)
    );
    if (!type::isObjectNull(env, exact)) {
        config->exact = type::getBooleanValue(env, exact);
        env->DeleteLocalRef(exact);
    }

    // use_delta_palette
    jobject use_delta_palette = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigUseDeltaPaletteFieldID.get(env)
    );
    if (!type::isObjectNull(env, use_delta_palette)) {
        config->use_delta_palette = type::getBooleanValue(env, use_delta_palette);
        env->DeleteLocalRef(use_delta_palette);
    }

    // use_sharp_yuv
    jobject use_sharp_yuv = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigUseSharpYUVFieldID.get(env)
    );
    if (!type::isObjectNull(env, use_sharp_yuv)) {
        config->use_sharp_yuv = type::getBooleanValue(env, use_sharp_yuv);
        env->DeleteLocalRef(use_sharp_yuv);
    }

    // qmin
    jobject qmin = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigQMinFieldID.get(env)
    );
    if (!type::isObjectNull(env, qmin)) {
        config->qmin = type::getIntegerValue(env, qmin);
        env->DeleteLocalRef(qmin);
    }

    // qmax
    jobject qmax = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigQMaxFieldID.get(env)
    );
    if (!type::isObjectNull(env, qmax)) {
        config->qmax = type::getIntegerValue(env, qmax);
        env->DeleteLocalRef(qmax);
    }
}

float enc::parseWebPQuality(JNIEnv *env, jobject jconfig) {
    jobject quality_field = env->GetObjectField(
            jconfig,
            ClassRegistry::webPConfigQualityFieldID.get(env)
    );
    float quality;
    if (type::isObjectNull(env, quality_field)) {
        quality = 70.0f;
    } else {
        quality = type::getFloatValue(env, quality_field);
        env->DeleteLocalRef(quality_field);
    }
    return quality;
}

void enc::parseEncoderOptions(
        JNIEnv *env,
        jobject joptions,
        WebPAnimEncoderOptions *options
) {
    // minimize_size
    jobject minimize_size = env->GetObjectField(
            joptions,
            ClassRegistry::webPAnimEncoderOptionsMinimizeSizeFieldID.get(env)
    );
    if (!type::isObjectNull(env, minimize_size)) {
        options->minimize_size = type::getBooleanValue(env, minimize_size);
        env->DeleteLocalRef(minimize_size);
    }

    // kmin
    jobject kmin = env->GetObjectField(
            joptions,
            ClassRegistry::webPAnimEncoderOptionsKMinFieldID.get(env)
    );
    if (!type::isObjectNull(env, kmin)) {
        options->kmin = type::getIntegerValue(env, kmin);
        env->DeleteLocalRef(kmin);
    }

    // kmax
    jobject kmax = env->GetObjectField(
            joptions,
            ClassRegistry::webPAnimEncoderOptionsKMaxFieldID.get(env)
    );
    if (!type::isObjectNull(env, kmax)) {
        options->kmax = type::getIntegerValue(env, kmax);
        env->DeleteLocalRef(kmax);
    }

    // allowMixed
    jobject allow_mixed = env->GetObjectField(
            joptions,
            ClassRegistry::webPAnimEncoderOptionsAllowMixedFieldID.get(env)
    );
    if (!type::isObjectNull(env, allow_mixed)) {
        options->allow_mixed = type::getBooleanValue(env, allow_mixed);
        env->DeleteLocalRef(allow_mixed);
    }

    // verbose
    jobject verbose = env->GetObjectField(
            joptions,
            ClassRegistry::webPAnimEncoderOptionsVerboseFieldID.get(env)
    );
    if (!type::isObjectNull(env, verbose)) {
        options->verbose = type::getBooleanValue(env, verbose);
        env->DeleteLocalRef(verbose);
    }

    // anim params
    WebPMuxAnimParams params;
    jobject jparams = env->GetObjectField(
            joptions,
            ClassRegistry::webPAnimEncoderOptionsAnimParamsFieldID.get(env)
    );
    if (!type::isObjectNull(env, jparams)) {
        // bgColor
        jobject bg_color = env->GetObjectField(
                jparams,
                ClassRegistry::webPMuxAnimParamsBgColorFieldID.get(env)
        );
        if (!type::isObjectNull(env, bg_color)) {
            params.bgcolor = type::getIntegerValue(env, bg_color);
            env->DeleteLocalRef(bg_color);
        } else {
            params.bgcolor = 0;
        }

        // loop_count
        jobject loop_count = env->GetObjectField(
                jparams,
                ClassRegistry::webPMuxAnimParamsLoopCountFieldID.get(env)
        );
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