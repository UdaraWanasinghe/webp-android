//
// Created by udara on 11/5/21.
//

#include "includes/decoder.h"
#include "webp/demux.h"
#include "includes/helper.h"

class Decoder {

public:
    static void ParseOptions(JNIEnv *env, jobject options, WebPAnimDecoderOptions *decoder_options) {
        jclass options_class = env->GetObjectClass(options);
        // color mode
        jobject color_mode = env->GetObjectField(options, env->GetFieldID(options_class, "colorMode", "Ljava/lang/Integer;"));
        if (color_mode != nullptr) {
            decoder_options->color_mode = (WEBP_CSP_MODE) GetIntegerValue(env, color_mode);
        }
        // use threads
        jobject use_threads = env->GetObjectField(options, env->GetFieldID(options_class, "useThreads", "Ljava/lang/Boolean;"));
        if (use_threads != nullptr) {
            decoder_options->use_threads = GetBooleanValue(env, use_threads);
        }
    }
};

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpcodec_decoder_WebPAnimDecoder_start(JNIEnv *env, jobject self, jobject options) {
    WebPAnimDecoderOptions decoder_options;
    if (WebPAnimDecoderOptionsInit(&decoder_options)) {
        Decoder::ParseOptions(env, options, &decoder_options);

    } else {
        ThrowException(env, "WebPAnimDecoderOptionsInit failed");
    }
}