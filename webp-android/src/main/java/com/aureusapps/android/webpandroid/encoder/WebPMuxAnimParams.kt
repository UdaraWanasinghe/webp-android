package com.aureusapps.android.webpandroid.encoder

/**
 * The `WebPMuxAnimParams` class represents animation parameters for a WebP muxer.
 *
 * @property backgroundColor The background color of the canvas stored as an integer value. The color is stored in MSB order where bits 00-07 represent the alpha channel, bits 08-15 represent the red channel, bits 16-23 represent the green channel, and bits 24-31 represent the blue channel.
 * @property loopCount The number of times to repeat the animation. Use 0 for infinite looping.
 */
data class WebPMuxAnimParams(
    val backgroundColor: Int? = null,
    val loopCount: Int? = null
)