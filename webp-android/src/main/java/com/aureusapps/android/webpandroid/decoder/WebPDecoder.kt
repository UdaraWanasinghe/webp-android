package com.aureusapps.android.webpandroid.decoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import java.util.concurrent.CancellationException

/**
 * The [WebPDecoder] class provides functionality for decoding WebP images.
 */
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

    private fun notifyFrameDecoded(index: Int, timestamp: Long, frame: Bitmap, uri: Uri?) {
        decodeListeners.forEach {
            it.onFrameDecoded(index, timestamp, frame, uri)
        }
    }

    /**
     * Adds a decode listener to receive decoding events.
     *
     * @param listener The decode listener to add.
     *
     * @return `true` if the listener was added successfully, `false` otherwise.
     */
    fun addDecodeListener(listener: WebPDecodeListener): Boolean {
        return decodeListeners.add(listener)
    }

    /**
     * Removes a decode listener.
     *
     * @param listener The decode listener to remove.
     *
     * @return `true` if the listener was removed successfully, `false` otherwise.
     */
    fun removeDecodeListener(listener: WebPDecodeListener): Boolean {
        return decodeListeners.remove(listener)
    }

    /**
     * Decodes all frames of a WebP image and optionally saves them to a destination [Uri].
     *
     * @param context The Android [Context] object.
     * @param srcUri The [Uri] of the source WebP image.
     * @param dstUri The [Uri] of the destination directory to save the decoded frames (optional). This could be a file [Uri] or a tree [Uri] returned from [Intent.ACTION_OPEN_DOCUMENT_TREE].
     *
     * @throws [RuntimeException] If decoding error occurred.
     * @throws [CancellationException] If user cancelled the decoding process.
     */
    external fun decodeFrames(context: Context, srcUri: Uri, dstUri: Uri? = null)

    /**
     * Decodes the image information of a WebP image.
     *
     * @param context The Android [Context] object.
     * @param srcUri The [Uri] of the source WebP image.
     *
     * @return The [WebPInfo] object containing the decoded image information.
     * @throws [RuntimeException] If error occurred.
     */
    external fun decodeInfo(context: Context, srcUri: Uri): WebPInfo

    /**
     * Cancels the decoding process.
     */
    external fun cancel()

    /**
     * Releases the resources used by the [WebPDecoder] object.
     */
    external fun release()

}