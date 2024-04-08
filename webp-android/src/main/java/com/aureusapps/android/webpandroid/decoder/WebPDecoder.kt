package com.aureusapps.android.webpandroid.decoder

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import com.aureusapps.android.webpandroid.CodecException
import com.aureusapps.android.webpandroid.CodecResult
import com.aureusapps.android.webpandroid.utils.CodecHelper
import com.getkeepsafe.relinker.ReLinker

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

    private external fun nativeDecodeNextFrame(): FrameDecodeResult

    private external fun nativeDecodeFrames(context: Context, dstUri: Uri?): Int

    private external fun nativeReset()

    private external fun nativeCancel()

    private external fun nativeRelease()

    private fun notifyInfoDecoded(info: WebPInfo) {
        decodeListeners.forEach { listener ->
            listener.onInfoDecoded(info)
        }
    }

    private fun notifyFrameDecoded(
        index: Int,
        timestamp: Long,
        frame: Bitmap,
        uri: Uri,
    ) {
        decodeListeners.forEach { listener ->
            listener.onFrameDecoded(index, timestamp, frame, uri)
        }
    }

    private inline fun <reified T> handleResultCode(
        resultCode: Int,
        onSuccess: () -> T = { Unit as T },
    ): T {
        val codecResult = CodecHelper.resultCodeToCodecResult(resultCode)
        if (codecResult == CodecResult.SUCCESS) {
            return onSuccess()
        }
        throw CodecException(codecResult)
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

    /**
     * Sets the data source of the decoder.
     *
     * @param srcUri The URI of the WebP file to be decoded.
     * @throws CodecException if failed to set data source.
     */
    fun setDataSource(srcUri: Uri) {
        val resultCode = nativeSetDataSource(context, srcUri)
        handleResultCode<Unit>(resultCode)
    }

    /**
     * Decodes the image information of a WebP image.
     *
     * @return The [WebPInfo] object containing the decoded image information.
     * @throws CodecException if failed to decode WebP info or [RuntimeException] if unexpected error occurred.
     */
    fun decodeInfo(): WebPInfo {
        val decodeResult = nativeDecodeInfo()
        return handleResultCode(decodeResult.resultCode) {
            decodeResult.webPInfo ?: throw RuntimeException("Unexpected null result: webPInfo is null")
        }
    }

    /**
     * Decodes the next frame of the WebP.
     *
     * @return The result of decoding the next frame in a [FrameDecodeResult] object. If the decoder reaches the end
     * of the content, [FrameDecodeResult.frame] will be null, and [FrameDecodeResult.resultCode] will be equal to
     * [CodecResult.ERROR_NO_MORE_FRAMES].
     *
     * @throws CodecException if the decoding of the next frame fails.
     */
    fun decodeNextFrame(): FrameDecodeResult {
        val decodeResult = nativeDecodeNextFrame()
        val codecResult = CodecHelper.resultCodeToCodecResult(decodeResult.resultCode)
        if (codecResult != CodecResult.SUCCESS && codecResult != CodecResult.ERROR_NO_MORE_FRAMES) {
            throw CodecException(codecResult)
        }
        return decodeResult
    }

    /**
     * Decodes all frames of a WebP image and optionally saves them to a destination [Uri].
     *
     * @param dstUri The [Uri] of the destination directory to save the decoded frames (optional). This could be a file [Uri] or a tree [Uri] returned from [Intent.ACTION_OPEN_DOCUMENT_TREE].
     *
     * @throws [CodecException] with [CodecException.codecResult] equal to [CodecResult.ERROR_USER_ABORT] If user cancelled the decoding process.
     */
    fun decodeFrames(dstUri: Uri? = null) {
        val resultCode = nativeDecodeFrames(context, dstUri)
        handleResultCode<Unit>(resultCode)
    }

    /**
     * Resets the decoder's state to its initial configuration and sets the current frame index to 0.
     */
    fun reset() {
        nativeReset()
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