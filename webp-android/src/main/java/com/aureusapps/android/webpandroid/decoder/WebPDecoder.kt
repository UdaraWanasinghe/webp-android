package com.aureusapps.android.webpandroid.decoder

import android.content.Context
import android.net.Uri

object WebPDecoder {

    init {
        System.loadLibrary("webpcodec_jni")
    }

    external fun extractImages(
        context: Context,
        sourceUri: Uri,
        decodeListener: WebPDecoderListener
    )

    external fun readInfo(filePath: String): WebPInfo

}