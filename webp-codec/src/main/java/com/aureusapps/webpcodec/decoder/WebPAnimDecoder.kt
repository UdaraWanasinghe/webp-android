package com.aureusapps.webpcodec.decoder

class WebPAnimDecoder(
    val path: String,
    val listener: WebPAnimDecoderListener
) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    external fun start(options: WebPAnimDecoderOptions)

}