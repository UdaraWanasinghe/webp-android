package com.aureusapps.android.webpandroid.extensions

import android.graphics.Bitmap
import android.net.Uri
import com.aureusapps.android.webpandroid.decoder.WebPDecodeListener
import com.aureusapps.android.webpandroid.decoder.WebPDecoder
import com.aureusapps.android.webpandroid.decoder.WebPInfo

/**
 * Adds a decode listener to the [WebPDecoder].
 *
 * @param onInfoDecoded A lambda function that will be called when the WebP image's
 *                      information is decoded.
 * @param onFrameDecoded A lambda function that will be called when a frame of the WebP
 *                       image is decoded.
 *
 * @return A [WebPDecodeListener] that can be used to remove the added listener when necessary.
 */
inline fun WebPDecoder.addDecodeListener(
    crossinline onInfoDecoded: (info: WebPInfo) -> Unit = {},
    crossinline onFrameDecoded: (index: Int, timestamp: Long, bitmap: Bitmap, uri: Uri) -> Unit = { _, _, _, _ -> }
): WebPDecodeListener {
    val listener = object : WebPDecodeListener {
        override fun onInfoDecoded(info: WebPInfo) {
            onInfoDecoded(info)
        }

        override fun onFrameDecoded(index: Int, timestamp: Long, bitmap: Bitmap, uri: Uri) {
            onFrameDecoded(index, timestamp, bitmap, uri)
        }
    }
    addDecodeListener(listener)
    return listener
}