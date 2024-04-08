package com.aureusapps.android.webpandroid.decoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import com.aureusapps.android.webpandroid.CodecException
import com.aureusapps.android.webpandroid.CodecResult
import com.aureusapps.android.webpandroid.utils.CodecHelper
import com.aureusapps.android.webpandroid.utils.Logger
import com.getkeepsafe.relinker.ReLinker
import java.util.concurrent.CancellationException

/**
 * The [WebPDecoder] class provides functionality for decoding WebP images.
 */
class WebPDecoder(private val context: Context) {

    companion object {
        private const val TAG = "WebPDecoder"
    }

    init {
        ReLinker.loadLibrary(context, "webpcodec_jni")
    }

    private val nativePointer: Long
    private val decodeListeners = mutableSetOf<WebPDecodeListener>()

    init {
        nativePointer = nativeCreate()
    }

    private external fun nativeCreate(): Long

    private external fun nativeConfigure(config: DecoderConfig)

    private external fun nativeSetDataSource(context: Context, srcUri: Uri): Int

    private external fun nativeDecodeInfo(): InfoDecodeResult

    /**
     * Returns bitmap if decode successful. Null if failed to decode frame.
     * This function automatically resets the decoder if it is at the end before starting decoding process.
     */
    private external fun nativeDecodeNextFrame(): FrameDecodeResult

    private external fun nativeDecodeFrames(
        context: Context,
        dstUri: Uri?,
    ): Int

    private external fun nativeResetDecoder()

    private external fun nativeCancel()

    private external fun nativeRelease()

    private fun notifyInfoDecoded(info: WebPInfo) {
        decodeListeners.forEach { listener ->
            listener.onInfoDecoded(info)
        }
    }

    private fun notifyFrameDecoded(index: Int, timestamp: Long, frame: Bitmap, uri: Uri) {
        decodeListeners.forEach { listener ->
            listener.onFrameDecoded(index, timestamp, frame, uri)
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
     * Configures this webp decoder.
     *
     * @param config DecoderConfig to apply.
     * @return this WebPDecoder instance.
     */
    fun configure(config: DecoderConfig): WebPDecoder {
        nativeConfigure(config)
        return this
    }

    fun setDataSource(srcUri: Uri) {
        nativeSetDataSource(context, srcUri)
    }

    /**
     * Decodes the image information of a WebP image.
     *
     * @return The [WebPInfo] object containing the decoded image information.
     * @throws [RuntimeException] If error occurred.
     */
    fun decodeInfo(): WebPInfo {
        val decodeResult = nativeDecodeInfo()
        if (decodeResult.webPInfo != null) {
            return decodeResult.webPInfo
        }
        val codecResult = CodecHelper.resultCodeToCodecResult(decodeResult.resultCode)
        throw CodecException(codecResult)
    }

    fun decodeNextFrame(): FrameDecodeResult {
        val decodeResult = nativeDecodeNextFrame()
        val codecResult = CodecHelper.resultCodeToCodecResult(decodeResult.resultCode)
        if (codecResult != CodecResult.SUCCESS) {
            Logger.e(TAG, codecResult.message)
        }
        return decodeResult
    }

    /**
     * Decodes all frames of a WebP image and optionally saves them to a destination [Uri].
     *
     * @param srcUri The [Uri] of the source WebP image.
     * @param dstUri The [Uri] of the destination directory to save the decoded frames (optional). This could be a file [Uri] or a tree [Uri] returned from [Intent.ACTION_OPEN_DOCUMENT_TREE].
     *
     * @throws [RuntimeException] If decoding error occurred.
     * @throws [CancellationException] If user cancelled the decoding process.
     */
    fun decodeFrames(dstUri: Uri? = null) {
        nativeDecodeFrames(context, dstUri)
    }

    fun resetDecoder() {
        nativeResetDecoder()
    }

    /**
     * Cancels the decoding process.
     */
    fun cancel() {
        nativeCancel()
    }

    /**
     * Releases the resources used by the [WebPDecoder] object.
     */
    fun release() {
        nativeRelease()
    }

}