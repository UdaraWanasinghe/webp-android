package com.aureusapps.webpandroid.encoder

data class WebPMuxAnimParams(

    /**
     * Background color of the canvas stored (in MSB order) as:
     * Bits 00 to 07: Alpha.
     * Bits 08 to 15: Red.
     * Bits 16 to 23: Green.
     * Bits 24 to 31: Blue.
     */
    val backgroundColor: Int? = null,

    /**
     * Number of times to repeat the animation [0 = infinite].
     */
    val loopCount: Int? = null,
)