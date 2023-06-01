package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap

interface WebPDecoderListener {

    /**
     * Called when the decoder has decoded information about the image.
     *
     * @param info The information about the image.
     */
    fun onReceiveInfo(info: WebPInfo)

    /**
     * Called when the decoder has decoded a frame of the image.
     *
     * @param frame The frame of the image.
     * @param timestamp The timestamp of the frame.
     */
    fun onReceiveFrame(frame: Bitmap, timestamp: Int)

}