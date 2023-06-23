package com.aureusapps.android.webpandroid.encoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri

/**
 * Constructs a WebP animation encoder with the specified width, height, and options.
 *
 * @param width The width of the animation frames in pixels.
 * @param height The height of the animation frames in pixels.
 * @param options The optional encoding options for the WebP animation. Default value is null.
 */
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

    /**
     * Adds a progress listener to the WebP animation encoder.
     *
     * @param listener The progress listener to be added.
     *
     * @return true if the listener was successfully added, false otherwise.
     */
    fun addProgressListener(listener: WebPAnimEncoderProgressListener): Boolean {
        return progressListeners.add(listener)
    }

    /**
     * Removes a progress listener from the WebP animation encoder.
     *
     * @param listener The progress listener to be removed.
     *
     * @return true if the listener was successfully removed, false otherwise.
     */
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

    /**
     * Configures the WebP encoder with the specified configuration.
     *
     * @param config The WebP configuration to be applied.
     * @param preset The optional preset to be used for configuring the encoder. Default value is null.
     */
    external fun configure(
        config: WebPConfig,
        preset: WebPPreset? = null
    )

    /**
     * Adds a frame to the WebP animation with the specified timestamp and source bitmap.
     *
     * @param timestamp The timestamp of the frame.
     * @param srcBitmap The source bitmap to be added as a frame.
     */
    external fun addFrame(timestamp: Long, srcBitmap: Bitmap)

    /**
     * Adds a frame to the WebP animation with the specified timestamp and source Uri.
     *
     * @param context The Android context.
     * @param timestamp The timestamp of the frame.
     * @param srcUri The source Uri pointing to the image file to be added as a frame.
     * This could be an Android content provider Uri, file Uri, Android resource Uri or a http Uri.
     */
    external fun addFrame(context: Context, timestamp: Long, srcUri: Uri)

    /**
     * Assembles the WebP animation and saves it to the specified destination Uri.
     *
     * @param context The Android context.
     * @param timestamp The end timestamp of the animation.
     * @param dstUri The destination Uri where the WebP animation will be saved.
     * This could be a content provider Uri returned by [Intent.ACTION_CREATE_DOCUMENT] or a file Uri.
     */
    external fun assemble(context: Context, timestamp: Long, dstUri: Uri)

    /**
     * Cancels the ongoing WebP animation encoding process.
     */
    external fun cancel()

    /**
     * Releases any resources associated with the WebP encoder.
     */
    external fun release()

}