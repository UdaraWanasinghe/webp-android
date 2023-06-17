package com.aureusapps.android.webpandroid.encoder

import android.content.Context
import android.graphics.Bitmap
import android.net.Uri

class WebPEncoder(width: Int, height: Int) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    private val progressListeners = mutableSetOf<WebPEncoderProgressListener>()
    private val nativePointer: Long

    init {
        nativePointer = create(width, height)
        if (nativePointer == 0L) {
            throw RuntimeException("Failed to create native encoder")
        }
    }

    private external fun create(width: Int, height: Int): Long

    external fun configure(config: WebPConfig, preset: WebPPreset? = null)

    fun addProgressListener(listener: WebPEncoderProgressListener) {
        progressListeners.add(listener)
    }

    fun removeProgressListener(listener: WebPEncoderProgressListener) {
        progressListeners.remove(listener)
    }

    private fun notifyProgressChanged(progress: Int): Boolean {
        var encode = true
        progressListeners.forEach {
            if (!it.onProgressChanged(progress)) {
                encode = false
            }
        }
        return encode
    }

    external fun encode(context: Context, srcBitmap: Bitmap, dstUri: Uri)

    external fun encode(context: Context, srcUri: Uri, dstUri: Uri)

    external fun cancel()

    external fun release()

}