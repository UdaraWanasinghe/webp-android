package com.aureusapps.android.webpandroid.encoder

import android.content.Context
import android.graphics.Bitmap
import android.net.Uri

class WebPAnimEncoder(
    width: Int,
    height: Int,
    options: WebPAnimEncoderOptions? = null
) {

    companion object {
        init {
            System.loadLibrary("webpcodec_jni")
        }
    }

    private val nativePointer: Long

    private val progressListeners: ArrayList<WebPAnimEncoderProgressListener> = ArrayList()

    init {
        nativePointer = create(width, height, options)
        if (nativePointer == 0L) {
            throw RuntimeException("Failed to create native encoder")
        }
    }

    fun addProgressListener(listener: WebPAnimEncoderProgressListener): Boolean {
        return progressListeners.add(listener)
    }

    fun removeProgressListener(listener: WebPAnimEncoderProgressListener): Boolean {
        return progressListeners.remove(listener)
    }

    private fun notifyProgressChanged(currentFrame: Int, frameProgress: Int): Boolean {
        var encode = true
        progressListeners.forEach {
            if (!it.onProgressChanged(currentFrame, frameProgress)) {
                encode = false
            }
        }
        return encode
    }

    private external fun create(
        width: Int,
        height: Int,
        options: WebPAnimEncoderOptions?
    ): Long

    external fun configure(
        config: WebPConfig,
        preset: WebPPreset? = null
    )

    external fun addFrame(timestamp: Long, srcBitmap: Bitmap)

    external fun addFrame(context: Context, timestamp: Long, srcUri: Uri)

    external fun assemble(context: Context, timestamp: Long, dstUri: Uri)

    external fun cancel()

    external fun release()

}