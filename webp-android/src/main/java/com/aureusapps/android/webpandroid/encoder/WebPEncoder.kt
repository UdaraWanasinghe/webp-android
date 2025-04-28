package com.aureusapps.android.webpandroid.encoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import com.aureusapps.android.webpandroid.utils.BitmapUtils
import com.getkeepsafe.relinker.ReLinker
import java.util.concurrent.CancellationException

/**
 * The [WebPEncoder] class provides functionality for encoding images to the WebP format.
 *
 * @param context The Android context.
 * @param width The width of the image to be encoded. If negative, original bitmap width will be used.
 * @param height The height of the image to be encoded. If negative, original bitmap height will be used.
 */
class WebPEncoder(private val context: Context, width: Int = -1, height: Int = -1) {

    init {
        ReLinker.loadLibrary(context, "webpcodec_jni")
    }

    private val progressListeners = mutableSetOf<WebPEncoderProgressListener>()
    private val nativePointer: Long

    init {
        nativePointer = nativeCreate(width, height)
        if (nativePointer == 0L) {
            throw RuntimeException("Failed to create native encoder.")
        }
    }

    private external fun nativeCreate(
        width: Int,
        height: Int,
    ): Long

    private external fun nativeConfigure(
        config: WebPConfig?,
        preset: WebPPreset?,
    )

    private external fun nativeEncode(
        context: Context,
        srcBitmap: Bitmap,
        dstUri: Uri,
    )

    private external fun nativeCancel()

    private external fun nativeRelease()

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

    /**
     * Configures the WebP encoder.
     *
     * @return this encoder instance.
     *
     * @param config The WebP configuration object.
     * @param preset The optional WebP preset configuration.
     */
    fun configure(config: WebPConfig? = null, preset: WebPPreset? = null): WebPEncoder {
        if (config == null && preset == null) {
            throw IllegalStateException("Both config and preset cannot be null at the same time")
        }
        nativeConfigure(config, preset)
        return this
    }

    /**
     * Encodes an image file from the given source [Uri] and saves the result to the specified destination [Uri].
     *
     * @param srcUri The source [Uri] of the image file to encode. This could be a content provider [Uri], file [Uri], Android resource [Uri] or a http [Uri].
     * @param dstUri The destination [Uri] to save the encoded image. This could be a content provider [Uri] returned by [Intent.ACTION_CREATE_DOCUMENT] or a file [Uri].
     *
     * @return this encoder instance.
     *
     * @throws [RuntimeException] If encoding error occurred.
     * @throws [CancellationException] If encoding process cancelled.
     *
     */
    fun encode(srcUri: Uri, dstUri: Uri): WebPEncoder {
        val srcBitmap = BitmapUtils.decodeUri(context, srcUri)
            ?: throw RuntimeException("Failed to decode bitmap from uri.")
        nativeEncode(context, srcBitmap, dstUri)
        srcBitmap.recycle()
        return this
    }

    /**
     * Encodes a [Bitmap] image and saves the result to the specified destination [Uri].
     *
     * @param srcBitmap The source [Bitmap] image to encode.
     * @param dstUri The destination [Uri] to save the encoded image. This could be a content provider [Uri] returned by [Intent.ACTION_CREATE_DOCUMENT] or a file [Uri].
     *
     * @return this encoder instance.
     *
     * @throws [RuntimeException] If encoding error occurred.
     * @throws [CancellationException] If encoding process cancelled.
     */
    fun encode(srcBitmap: Bitmap, dstUri: Uri): WebPEncoder {
        nativeEncode(context, srcBitmap, dstUri)
        return this
    }

    /**
     * Cancels the ongoing encoding process.
     */
    fun cancel() {
        nativeCancel()
    }

    /**
     * Releases the resources associated with the [WebPEncoder] instance.
     */
    fun release() {
        nativeRelease()
    }

}