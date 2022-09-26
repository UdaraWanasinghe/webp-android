//
// Created by udara on 11/5/21.
//

#include "webp/demux.h"
#include "includes/helper.h"

enum StatusFlag {
    START,
    STOP
};

class Decoder {

public:
    StatusFlag status_flag = START;
    WebPAnimDecoder *anim_decoder = nullptr;
    jweak decoder_obj = nullptr;

    Decoder(const char *path, WebPAnimDecoderOptions decoder_options) {
        // check path is file or url
        // create thread
        // load data to byte buffer
        // decode byte buffer
        // render frame with timestamp
    }

    static Decoder *GetInstance(JNIEnv *env, jobject self) {
        jclass cls = env->GetObjectClass(self);
        if (!cls)
            ThrowException(env, "GetObjectClass failed");
        jfieldID nativeObjectPointerID = env->GetFieldID(cls, "nativeObjectPointer", "J");
        if (!nativeObjectPointerID)
            ThrowException(env, "GetFieldID failed");
        jlong nativeObjectPointer = env->GetLongField(self, nativeObjectPointerID);
        return reinterpret_cast<Decoder *>(nativeObjectPointer);
    }

    void DecodeFile(JNIEnv *env, char *path, WebPAnimDecoderOptions *decoder_options) {

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
        WebPAnimDecoder *decoder = WebPAnimDecoderNew(&webp_data, decoder_options);
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

    }

    static const char *GetPath(JNIEnv *env, jobject self) {
        // get path
        jclass decoder_class = env->GetObjectClass(self);
        jfieldID path_field_id = env->GetFieldID(decoder_class, "path", "Ljava/lang/String;");
        auto path_str = (jstring) env->GetObjectField(self, path_field_id);
        return env->GetStringUTFChars(path_str, nullptr);
    }

    static void
    ParseOptions(JNIEnv *env, jobject options_obj, WebPAnimDecoderOptions *decoder_options) {
        jclass options_class = env->GetObjectClass(options_obj);
        // color mode
        jobject color_mode = env->GetObjectField(options_obj,
                                                 env->GetFieldID(options_class, "colorMode",
                                                                 "Ljava/lang/Integer;"));
        if (color_mode != nullptr) {
            decoder_options->color_mode = (WEBP_CSP_MODE) GetIntegerValue(env, color_mode);
        }
        // use threads
        jobject use_threads = env->GetObjectField(options_obj,
                                                  env->GetFieldID(options_class, "useThreads",
                                                                  "Ljava/lang/Boolean;"));
        if (use_threads != nullptr) {
            decoder_options->use_threads = GetBooleanValue(env, use_threads);
        }
    }
};

extern "C"
JNIEXPORT jlong JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_create(JNIEnv *env, jobject self,
                                                                       jobject options_obj) {
    WebPAnimDecoderOptions decoder_options;
    if (WebPAnimDecoderOptionsInit(&decoder_options)) {
        Decoder::ParseOptions(env, options_obj, &decoder_options);
        const char *path = Decoder::GetPath(env, self);
        auto *decoder = new Decoder(path, decoder_options);
        return reinterpret_cast<jlong>(decoder);
    } else {
        ThrowException(env, "WebPAnimDecoderOptionsInit failed");
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_start(JNIEnv
                                                                      *env,
                                                                      jobject self
) {
    auto *decoder = Decoder::GetInstance(env, self);
// create thread
// read file
// decode and render to surface
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_stop(JNIEnv
                                                                     *env,
                                                                     jobject self
) {
    auto *decoder = Decoder::GetInstance(env, self);
    decoder->status_flag = STOP;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_aureusapps_webpandroid_decoder_WebPAnimDecoder_release(JNIEnv
                                                                        *env,
                                                                        jobject self
) {
    auto *decoder = Decoder::GetInstance(env, self);
    if (decoder != nullptr) {
        WebPAnimDecoderDelete(decoder->anim_decoder);
        jclass decoder_class = env->GetObjectClass(self);
        env->SetLongField(self,
                          env->GetFieldID(decoder_class, "nativeObjectPointer", "J"),
                          (jlong) 0);
        env->DeleteWeakGlobalRef(decoder->decoder_obj);
        delete decoder;
    } else {
        ThrowException(env, "Decoder is null");
    }
}