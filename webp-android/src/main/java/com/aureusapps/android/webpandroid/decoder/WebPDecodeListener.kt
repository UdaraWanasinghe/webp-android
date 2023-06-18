package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap

interface WebPDecodeListener {

    /**
     * Called when the decoder has decoded information about the image.
     *
     * @param info The information about the image.
     */
    fun onInfoDecoded(info: WebPInfo)

    /**
     * Called when the decoder has decoded a frame of the image.
     *
     * @param frame The frame of the image.
     * @param timestamp The timestamp of the frame.
     */
    fun onFrameDecoded(index: Int, timestamp: Long, frame: Bitmap)

}