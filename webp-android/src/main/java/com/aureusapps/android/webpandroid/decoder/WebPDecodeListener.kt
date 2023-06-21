package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap
import android.net.Uri

interface WebPDecodeListener {

    /**
     * Called when the decoder has decoded information about the image.
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
     * @param uri Uri of the saved frame or null if the frame is not saved.
     */
    fun onFrameDecoded(index: Int, timestamp: Long, bitmap: Bitmap, uri: Uri?)

}