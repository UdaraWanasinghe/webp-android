package com.aureusapps.android.webpandroid.encoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import com.getkeepsafe.relinker.ReLinker

/**
 * Constructs a WebP animation encoder with the specified width, height, and options.
 *
 * @param context The Android context.
 * @param width The width of the animation frames in pixels. If negative, width of the first bitmap will be used.
 * @param height The height of the animation frames in pixels. If negative, height of the first bitmap will be used.
 * @param options The optional encoding options for the WebP animation. Default value is null.
 */
class WebPAnimEncoder(
    private val context: Context,
    width: Int = -1,
    height: Int = -1,
    options: WebPAnimEncoderOptions? = null
) {

    init {
        ReLinker.loadLibrary(context, "webpcodec_jni")
    }

    private val nativePointer: Long

    private val progressListeners: ArrayList<WebPAnimEncoderProgressListener> = ArrayList()

    init {
        nativePointer = nativeCreate(width, height, options)
        if (nativePointer == 0L) {
            throw RuntimeException("Failed to create native encoder")
        }
    }

    private external fun nativeCreate(
        width: Int,
        height: Int,
        options: WebPAnimEncoderOptions?
    ): Long

    private external fun nativeConfigure(
        config: WebPConfig?,
        preset: WebPPreset?
    )

    private external fun nativeAddFrame1(
        context: Context,
        timestamp: Long,
        srcUri: Uri
    )

    private external fun nativeAddFrame2(
        timestamp: Long,
        srcBitmap: Bitmap
    )

    private external fun nativeAssemble(
        context: Context,
        timestamp: Long,
        dstUri: Uri
    )

    private external fun nativeCancel()

    private external fun nativeRelease()

    private fun notifyProgressChanged(currentFrame: Int, frameProgress: Int): Boolean {
        var encode = true
        progressListeners.forEach {
            if (!it.onProgressChanged(currentFrame, frameProgress)) {
                encode = false
            }
        }
        return encode
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

    /**
     * Configures the WebP encoder with the specified configuration.
     *
     * @param config The WebP configuration to be applied.
     * @param preset The optional preset to be used for configuring the encoder.
     * @return this animation encoder instance.
     */
    fun configure(config: WebPConfig? = null, preset: WebPPreset? = null): WebPAnimEncoder {
        if (config == null && preset == null) {
            throw IllegalStateException("Both config and preset cannot be null at the same time")
        }
        nativeConfigure(config, preset)
        return this
    }

    /**
     * Adds a frame to the WebP animation with the specified timestamp and source Uri.
     *
     * @param timestamp The timestamp of the frame.
     * @param srcUri The source Uri pointing to the image file to be added as a frame. This could be an Android content provider Uri, file Uri, Android resource Uri or a http Uri.
     * @return this animation encoder instance.
     */
    fun addFrame(timestamp: Long, srcUri: Uri): WebPAnimEncoder {
        nativeAddFrame1(context, timestamp, srcUri)
        return this
    }

    /**
     * Adds a frame to the WebP animation with the specified timestamp and source bitmap.
     *
     * @param timestamp The timestamp of the frame.
     * @param srcBitmap The source bitmap to be added as a frame.
     * @return this animation encoder instance.
     */
    fun addFrame(timestamp: Long, srcBitmap: Bitmap): WebPAnimEncoder {
        nativeAddFrame2(timestamp, srcBitmap)
        return this
    }

    /**
     * Assembles the WebP animation and saves it to the specified destination Uri.
     *
     * @param timestamp The end timestamp of the animation.
     * @param dstUri The destination Uri where the WebP animation will be saved. This could be a content provider Uri returned by [Intent.ACTION_CREATE_DOCUMENT] or a file Uri.
     * @return this animation encoder instance.
     */
    fun assemble(timestamp: Long, dstUri: Uri): WebPAnimEncoder {
        nativeAssemble(context, timestamp, dstUri)
        return this
    }

    /**
     * Cancels the ongoing WebP animation encoding process.
     */
    fun cancel() {
        nativeCancel()
    }

    /**
     * Releases any resources associated with the WebP encoder.
     */
    fun release() {
        nativeRelease()
    }

}