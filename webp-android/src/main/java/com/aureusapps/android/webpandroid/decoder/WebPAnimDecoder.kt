package com.aureusapps.android.webpandroid.decoder

class WebPAnimDecoder(
    private val path: String,
    options: WebPAnimDecoderOptions,
    private val listener: WebPAnimDecoderListener
) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    private var nativeObjectPointer: Long = 0

    init {
        nativeObjectPointer = create(options)
    }

    private external fun create(options: WebPAnimDecoderOptions): Long

    external fun start()

    external fun stop()

    external fun release()

}