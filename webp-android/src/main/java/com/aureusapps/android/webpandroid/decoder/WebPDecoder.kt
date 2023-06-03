package com.aureusapps.android.webpandroid.decoder

object WebPDecoder {

    init {
        System.loadLibrary("webpcodec_jni")
    }

    external fun extractImages(filePath: String, decodeListener: WebPDecoderListener)

    external fun readInfo(filePath: String): WebPInfo

}