package com.aureusapps.android.webpandroid.encoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import java.util.concurrent.CancellationException

/**
 * The [WebPEncoder] class provides functionality for encoding images to the WebP format.
 *
 * @param width The width of the image to be encoded.
 * @param height The height of the image to be encoded.
 */
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

    /**
     * Configures the WebP encoder.
     *
     * @param config The WebP configuration object.
     * @param preset The optional WebP preset configuration.
     */
    external fun configure(config: WebPConfig, preset: WebPPreset? = null)

    /**
     * Adds a progress listener to receive encoding progress updates.
     *
     * @param listener The progress listener to add.
     *
     * @return true if added successfully otherwise false.
     */
    fun addProgressListener(listener: WebPEncoderProgressListener): Boolean {
        return progressListeners.add(listener)
    }

    /**
     * Removes a progress listener to stop receiving encoding progress updates.
     *
     * @param listener The progress listener to remove.
     *
     * @return true if removed successfully, otherwise false.
     */
    fun removeProgressListener(listener: WebPEncoderProgressListener): Boolean {
        return progressListeners.remove(listener)
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

    /**
     * Encodes a [Bitmap] image and saves the result to the specified destination [Uri].
     *
     * @param context The Android context.
     * @param srcBitmap The source [Bitmap] image to encode.
     * @param dstUri The destination [Uri] to save the encoded image. This could be a content provider [Uri] returned by [Intent.ACTION_CREATE_DOCUMENT] or a file [Uri].
     *
     * @throws [RuntimeException] If encoding error occurred.
     * @throws [CancellationException] If encoding process cancelled.
     */
    external fun encode(context: Context, srcBitmap: Bitmap, dstUri: Uri)

    /**
     * Encodes an image file from the given source [Uri] and saves the result to the specified destination [Uri].
     *
     * @param context The Android context.
     * @param srcUri The source [Uri] of the image file to encode. This could be a content provider [Uri], file [Uri], Android resource [Uri] or a http [Uri].
     * @param dstUri The destination [Uri] to save the encoded image. This could be a content provider [Uri] returned by [Intent.ACTION_CREATE_DOCUMENT] or a file [Uri].
     *
     * @throws [RuntimeException] If encoding error occurred.
     * @throws [CancellationException] If encoding process cancelled.
     */
    external fun encode(context: Context, srcUri: Uri, dstUri: Uri)

    /**
     * Cancels the ongoing encoding process.
     */
    external fun cancel()

    /**
     * Releases the resources associated with the [WebPEncoder] instance.
     */
    external fun release()

}