package com.aureusapps.android.webpandroid.encoder

/**
 * The `WebPMuxAnimParams` class represents animation parameters for a WebP muxer.
 */
data class WebPMuxAnimParams(
    /**
     * The background color of the canvas stored as an integer value.
     * The color is stored in MSB order where bits 00-07 represent the alpha channel,
     * bits 08-15 represent the red channel, bits 16-23 represent the green channel,
     * and bits 24-31 represent the blue channel.
     */
    val backgroundColor: Int? = null,

    /**
     * The number of times to repeat the animation. Use 0 for infinite looping.
     */
    val loopCount: Int? = null
)