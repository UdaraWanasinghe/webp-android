package com.aureusapps.webpcodec.decoder

class WebPAnimDecoder(
    path: String,
    listener: WebPAnimDecoderListener
) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    external fun start(options: WebPAnimDecoderOptions)

}