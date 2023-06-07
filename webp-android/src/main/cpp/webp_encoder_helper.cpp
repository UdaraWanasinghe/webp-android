//
// Created by udara on 6/5/23.
//

#include "include/webp_encoder_helper.h"
#include "include/exception_helper.h"
#include "include/type_helper.h"

WebPPreset parseWebPPreset(JNIEnv *env, jobject *object) {
    // get preset class
    jclass presetClass = env->FindClass("com/aureusapps/android/webpandroid/encoder/WebPPreset");

    // check instance
    if (!env->IsInstanceOf(*object, presetClass)) {
        throw std::runtime_error("Given object is not of type WebPPreset");
    }

    // get ordinal
    jfieldID ordinalFieldID = env->GetFieldID(presetClass, "value", "I");
    jint ordinal = env->GetIntField(*object, ordinalFieldID);

    return WebPPreset(ordinal);
}

void parseWebPConfig(JNIEnv *env, jobject *object, WebPConfig *config) {
    // lossless
    jobject lossless = getObjectField(env, object, "lossless", "Ljava/lang/Integer;");
    if (lossless != nullptr) {
        config->lossless = getIntegerValue(env, &lossless);
    }

    // quality
    jobject quality = getObjectField(env, object, "quality", "Ljava/lang/Float;");
    if (quality != nullptr) {
        config->quality = getFloatValue(env, &quality);
    }

    // method
    jobject method = getObjectField(env, object, "method", "Ljava/lang/Integer;");
    if (method != nullptr) {
        config->method = getIntegerValue(env, &method);
    }

    // target size
    jobject targetSize = getObjectField(env, object, "targetSize", "Ljava/lang/Integer;");
    if (targetSize != nullptr) {
        config->target_size = getIntegerValue(env, &targetSize);
    }

    // target PSNR
    jobject targetPSNR = getObjectField(env, object, "targetPSNR", "Ljava/lang/Float;");
    if (targetPSNR != nullptr) {
        config->target_PSNR = getFloatValue(env, &targetPSNR);
    }

    // segments
    jobject segments = getObjectField(env, object, "segments", "Ljava/lang/Integer;");
    if (segments != nullptr) {
        config->segments = getIntegerValue(env, &segments);
    }

    // snsStrength
    jobject snsStrength = getObjectField(env, object, "snsStrength", "Ljava/lang/Integer;");
    if (snsStrength != nullptr) {
        config->sns_strength = getIntegerValue(env, &snsStrength);
    }

    // filterStrength
    jobject filterStrength = getObjectField(
            env,
            object,
            "filterStrength",
            "Ljava/lang/Integer;"
    );
    if (filterStrength != nullptr) {
        config->filter_strength = getIntegerValue(env, &filterStrength);
    }

    // filterSharpness
    jobject filterSharpness = getObjectField(
            env,
            object,
            "filterSharpness",
            "Ljava/lang/Integer;"
    );
    if (filterSharpness != nullptr) {
        config->filter_sharpness = getIntegerValue(env, &filterSharpness);
    }

    // filterType
    jobject filterType = getObjectField(env, object, "filterType", "Ljava/lang/Integer;");
    if (filterType != nullptr) {
        config->filter_type = getIntegerValue(env, &filterType);
    }

    // autoFilter
    jobject autoFilter = getObjectField(env, object, "autoFilter", "Ljava/lang/Boolean;");
    if (autoFilter != nullptr) {
        config->autofilter = getBooleanValue(env, &autoFilter);
    }

    // alphaCompression
    jobject alphaCompression = getObjectField(
            env,
            object,
            "alphaCompression",
            "Ljava/lang/Integer;"
    );
    if (alphaCompression != nullptr) {
        config->alpha_compression = getIntegerValue(env, &alphaCompression);
    }

    // alphaFiltering
    jobject alphaFiltering = getObjectField(
            env,
            object,
            "alphaFiltering",
            "Ljava/lang/Integer;"
    );
    if (alphaFiltering != nullptr) {
        config->alpha_filtering = getIntegerValue(env, &alphaFiltering);
    }

    // alphaQuality
    jobject alphaQuality = getObjectField(
            env,
            object,
            "alphaQuality",
            "Ljava/lang/Integer;"
    );
    if (alphaQuality != nullptr) {
        config->alpha_quality = getIntegerValue(env, &alphaQuality);
    }

    // pass
    jobject pass = getObjectField(env, object, "pass", "Ljava/lang/Integer;");
    if (pass != nullptr) {
        config->pass = getIntegerValue(env, &pass);
    }

    // showCompressed
    jobject showCompressed = getObjectField(
            env,
            object,
            "showCompressed",
            "Ljava/lang/Boolean;"
    );
    if (showCompressed != nullptr) {
        config->show_compressed = getBooleanValue(env, &showCompressed);
    }

    // preprocessing
    jobject preprocessing = getObjectField(
            env,
            object,
            "preprocessing",
            "Ljava/lang/Integer;"
    );
    if (preprocessing != nullptr) {
        config->preprocessing = getIntegerValue(env, &preprocessing);
    }

    // partitions
    jobject partitions = getObjectField(env, object, "partitions", "Ljava/lang/Integer;");
    if (partitions != nullptr) {
        config->partitions = getIntegerValue(env, &partitions);
    }

    // partitionLimit
    jobject partitionLimit = getObjectField(
            env,
            object,
            "partitionLimit",
            "Ljava/lang/Integer;"
    );
    if (partitionLimit != nullptr) {
        config->partition_limit = getIntegerValue(env, &partitionLimit);
    }

    // emulateJPEGSize
    jobject emulateJPEGSize = getObjectField(
            env,
            object,
            "emulateJPEGSize",
            "Ljava/lang/Boolean;"
    );
    if (emulateJPEGSize != nullptr) {
        config->emulate_jpeg_size = getBooleanValue(env, &emulateJPEGSize);
    }

    // threadLevel
    jobject threadLevel = getObjectField(env, object, "threadLevel", "Ljava/lang/Integer;");
    if (threadLevel != nullptr) {
        config->thread_level = getIntegerValue(env, &threadLevel);
    }

    // lowMemory
    jobject lowMemory = getObjectField(env, object, "lowMemory", "Ljava/lang/Boolean;");
    if (lowMemory != nullptr) {
        config->low_memory = getBooleanValue(env, &lowMemory);
    }

    // nearLossless
    jobject nearLossless = getObjectField(
            env,
            object,
            "nearLossless",
            "Ljava/lang/Integer;"
    );
    if (nearLossless != nullptr) {
        config->near_lossless = getIntegerValue(env, &nearLossless);
    }

    // exact
    jobject exact = getObjectField(env, object, "exact", "Ljava/lang/Boolean;");
    if (exact != nullptr) {
        config->exact = getBooleanValue(env, &exact);
    }

    // useDeltaPalette
    jobject useDeltaPalette = getObjectField(
            env,
            object,
            "useDeltaPalette",
            "Ljava/lang/Boolean;"
    );
    if (useDeltaPalette != nullptr) {
        config->use_delta_palette = getBooleanValue(env, &useDeltaPalette);
    }

    // useSharpYUV
    jobject useSharpYUV = getObjectField(env, object, "useSharpYUV", "Ljava/lang/Boolean;");
    if (useSharpYUV != nullptr) {
        config->use_sharp_yuv = getBooleanValue(env, &useSharpYUV);
    }

    // qmin
    jobject qmin = getObjectField(env, object, "qmin", "Ljava/lang/Integer;");
    if (qmin != nullptr) {
        config->qmin = getIntegerValue(env, &qmin);
    }

    // qmax
    jobject qmax = getObjectField(env, object, "qmax", "Ljava/lang/Integer;");
    if (qmax != nullptr) {
        config->qmax = getIntegerValue(env, &qmax);
    }

}

float parseWebPQuality(JNIEnv *env, jobject *config) {
    jclass configClass = env->GetObjectClass(*config);
    jfieldID qualityFieldID = env->GetFieldID(configClass, "quality", "Ljava/lang/Float;");
    jobject qualityField = env->GetObjectField(*config, qualityFieldID);
    float quality;
    if (qualityField == nullptr) {
        quality = 100.0f;
    } else {
        quality = getFloatValue(env, &qualityField);
    }
    env->DeleteLocalRef(configClass);
    return quality;
}

void parseAnimEncoderOptions(JNIEnv *env, jobject *object, WebPAnimEncoderOptions *options) {
    // minimizeSize
    jobject minimizeSize = getObjectField(env, object, "minimizeSize", "Ljava/lang/Boolean;");
    if (minimizeSize != nullptr) {
        options->minimize_size = getBooleanValue(env, &minimizeSize);
    }

    // kmin
    jobject kmin = getObjectField(env, object, "kmin", "Ljava/lang/Integer;");
    if (kmin != nullptr) {
        options->kmin = getIntegerValue(env, &kmin);
    }

    // kmax
    jobject kmax = getObjectField(env, object, "kmax", "Ljava/lang/Integer;");
    if (kmax != nullptr) {
        options->kmax = getIntegerValue(env, &kmax);
    }

    // allowMixed
    jobject allow_mixed = getObjectField(env, object, "allowMixed", "Ljava/lang/Boolean;");
    if (allow_mixed != nullptr) {
        options->allow_mixed = getBooleanValue(env, &allow_mixed);
    }

    // verbose
    jobject verbose = getObjectField(env, object, "verbose", "Ljava/lang/Boolean;");
    if (verbose != nullptr) {
        options->verbose = getBooleanValue(env, &verbose);
    }

    // anim params
    WebPMuxAnimParams webPMuxAnimParams;
    jobject animParams = getObjectField(
            env,
            object,
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