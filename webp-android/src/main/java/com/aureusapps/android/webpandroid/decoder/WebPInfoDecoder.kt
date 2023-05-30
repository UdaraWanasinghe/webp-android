package com.aureusapps.android.webpandroid.decoder

object WebPInfoDecoder {

    init {
        System.loadLibrary("webpcodec_jni")
    }

    external fun getWebPFeatures(path: String): WebPBitstreamFeatures

}