//
// Created by udara on 11/2/21.
//

#include "includes/encoder.h"
#include "includes/helper.h"
#include "webp/mux.h"

class Encoder {
private:
    WebPAnimEncoder *encoder;

public:
    Encoder(int width, int height, WebPAnimEncoderOptions options) {
        encoder = WebPAnimEncoderNew(width, height, &options);
    }

    static WebPAnimEncoderOptions ParseOptions(JNIEnv *env, jobject options) {
        WebPAnimEncoderOptions encoderOptions;
        if (WebPAnimEncoderOptionsInit(&encoderOptions)) {
            if (options != nullptr) {
                jclass optionsClass = env->GetObjectClass(options);
                // minimizeSize
                jobject minimizeSize = env->GetObjectField(options,
                                                           env->GetFieldID(optionsClass, "minimizeSize", "Ljava/lang/Boolean;"));
                if (minimizeSize != nullptr) {
                    encoderOptions.minimize_size = getBooleanValue(env, minimizeSize);
                }
                // kmin
                jobject kmin = env->GetObjectField(options, env->GetFieldID(optionsClass, "kmin", "Ljava/lang/Integer;"));
                if (kmin != nullptr) {
                    encoderOptions.kmin = getIntegerValue(env, kmin);
                }
                // kmax
                jobject kmax = env->GetObjectField(options, env->GetFieldID(optionsClass, "kmax", "Ljava/lang/Integer;"));
                if (kmax != nullptr) {
                    encoderOptions.kmax = getIntegerValue(env, kmax);
                }
                // allowMixed
                jobject allowMixed = env->GetObjectField(options,
                                                         env->GetFieldID(optionsClass, "allowMixed", "Ljava/lang/Boolean;"));
                if (allowMixed != nullptr) {
                    encoderOptions.allow_mixed = getBooleanValue(env, allowMixed);
                }
                // verbose
                jobject verbose = env->GetObjectField(options, env->GetFieldID(optionsClass, "verbose", "Ljava/lang/Boolean;"));
                if (verbose != nullptr) {
                    encoderOptions.verbose = getBooleanValue(env, verbose);
                }
                // anim params
                WebPMuxAnimParams animParams;
                jobject animParamsObj = env->GetObjectField(options, env->GetFieldID(optionsClass, "animParams",
                                                                                     "Lcom/aureusapps/webpcodec/encoder/WebPMuxAnimParams"));
                if (animParamsObj != nullptr) {
                    jclass animParamsCls = env->GetObjectClass(animParamsObj);
                    // bgColor
                    jobject bgColor = env->GetObjectField(animParamsObj, env->GetFieldID(animParamsCls, "backgroundColor",
                                                                                         "Ljava/lang/Integer;"));
                    if (bgColor != nullptr) {
                        animParams.bgcolor = getIntegerValue(env, bgColor);
                    } else {
                        animParams.bgcolor = 0;
                    }
                    // loopCount
                    jobject loopCount = env->GetObjectField(animParamsObj,
                                                            env->GetFieldID(animParamsCls, "loopCount", "Ljava/lang/Integer;"));
                    if (loopCount != nullptr) {
                        animParams.loop_count = getIntegerValue(env, loopCount);
                    } else {
                        animParams.loop_count = 1;
                    }
                } else {
                    animParams.bgcolor = 0;
                    animParams.loop_count = 1;
                }
            }
        } else {
            env->FatalError("WebPAnimEncoderOptionsInit failed");
        }
        return encoderOptions;
    }
};

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_create(JNIEnv *env, jobject thiz, jint width, jint height,
                                                             jobject options) {
    WebPAnimEncoderOptions encoderOptions = Encoder::ParseOptions(env, options);
    WebPAnimEncoder *encoder = WebPAnimEncoderNew(width, height, &encoderOptions);
    return reinterpret_cast<jlong>(encoder);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_config__Lcom_aureusapps_webpcodec_encoder_WebPConfig_2(JNIEnv *env,
                                                                                                             jobject thiz,
                                                                                                             jobject config) {

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_aureusapps_webpcodec_encoder_WebPAnimEncoder_config__Lcom_aureusapps_webpcodec_encoder_WebPPreset_2F(JNIEnv *env,
                                                                                                              jobject thiz,
                                                                                                              jobject preset,
                                                                                                              jfloat quality) {
    // TODO: implement config()
}