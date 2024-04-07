package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap
import android.net.Uri

/**
 * The [WebPDecodeListener] interface defines callback methods to receive decoding events during WebP decoding.
 */
interface WebPDecodeListener {

    /**
     * Called when the decoder has decoded information about the image.
     * Image information is decoded at the start of the decoding process.
     *
     * @param info The information about the image.
     */
    fun onInfoDecoded(info: WebPInfo)

    /**
     * Called when the decoder has decoded a frame of the image.
     * Do not recycle [bitmap] as it is reused internally.
     *
     * @param index The index of the frame.
     * @param bitmap The frame of the image.
     * @param timestamp The timestamp of the frame.
     * @param uri Uri of the saved frame or [Uri.EMPTY] if the frame is not saved.
     */
    fun onFrameDecoded(index: Int, timestamp: Long, bitmap: Bitmap, uri: Uri)

}
