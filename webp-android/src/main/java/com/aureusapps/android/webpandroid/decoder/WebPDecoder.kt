package com.aureusapps.android.webpandroid.decoder

import android.content.Context
import android.graphics.Bitmap
import android.net.Uri

class WebPDecoder {

    init {
        System.loadLibrary("webpcodec_jni")
    }

    private val nativePointer: Long
    private val decodeListeners = mutableSetOf<WebPDecodeListener>()

    init {
        nativePointer = create()
    }

    private external fun create(): Long

    private fun notifyInfoDecoded(info: WebPInfo) {
        decodeListeners.forEach {
            it.onInfoDecoded(info)
        }
    }

    private fun notifyFrameDecoded(index: Int, timestamp: Long, frame: Bitmap) {
        decodeListeners.forEach {
            it.onFrameDecoded(index, timestamp, frame)
        }
    }

    fun addDecodeListener(listener: WebPDecodeListener): Boolean {
        return decodeListeners.add(listener)
    }

    fun removeDecodeListener(listener: WebPDecodeListener): Boolean {
        return decodeListeners.remove(listener)
    }

    external fun decodeFrames(context: Context, srcUri: Uri, dstUri: Uri? = null)

    external fun decodeInfo(context: Context, srcUri: Uri): WebPInfo

    external fun cancel()

    external fun release()

}