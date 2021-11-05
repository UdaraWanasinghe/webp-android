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
        // parse decoder options
        Decoder::ParseOptions(env, options, &decoder_options);
        // get path
        jclass decoder_class = env->GetObjectClass(self);
        jfieldID path_field_id = env->GetFieldID(decoder_class, "path", "Ljava/lang/String;");
        auto path_str = (jstring) env->GetObjectField(self, path_field_id);
        const char *path = env->GetStringUTFChars(path_str, nullptr);

        // read webp file
        FILE *file;
        long file_size;
        uint8_t *buffer;
        size_t result;

        file = fopen(path, "rb");
        if (file == nullptr) {
            ThrowException(env, "Cannot open the file");
            return;
        }

        // obtain file size:
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        // allocate memory to contain the whole file:
        buffer = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * file_size));
        if (buffer == nullptr) {
            ThrowException(env, "Memory error");
            return;
        }

        // copy the file into the buffer:
        result = fread(buffer, 1, file_size, file);
        if (result != file_size) {
            ThrowException(env, "Reading error");
            return;
        }

        // terminate
        fclose(file);

        // set data
        WebPData webp_data;
        WebPDataInit(&webp_data);
        webp_data.bytes = buffer;
        webp_data.size = file_size;

        // decode
        WebPAnimDecoder *decoder = WebPAnimDecoderNew(&webp_data, &decoder_options);
        WebPAnimInfo anim_info;
        WebPAnimDecoderGetInfo(decoder, &anim_info);
        for (uint32_t i = 0; i < anim_info.loop_count; ++i) {
            while (WebPAnimDecoderHasMoreFrames(decoder)) {
                uint8_t *buf;
                int timestamp;
                WebPAnimDecoderGetNext(decoder, &buf, &timestamp);
                // ... (Render 'buf' based on 'timestamp').
                // ... (Do NOT free 'buf', as it is owned by 'dec').
            }
            WebPAnimDecoderReset(decoder);
        }
        WebPAnimDecoderDelete(decoder);

    } else {
        ThrowException(env, "WebPAnimDecoderOptionsInit failed");
    }
}