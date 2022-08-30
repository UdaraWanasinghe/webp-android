package com.aureusapps.webpcodec.encoder

data class WebPAnimEncoderOptions(

    /**
     * If true, minimize the output size (slow). Implicitly
     * disables key-frame insertion.
     */
    val minimizeSize: Boolean? = null,

    /**
     * Minimum and maximum distance between consecutive key
     * frames in the output. The library may insert some key
     * frames as needed to satisfy this criteria.
     * Note that these conditions should hold: kmax > kmin
     * and kmin >= kmax / 2 + 1. Also, if kmax <= 0, then
     * key-frame insertion is disabled; and if kmax == 1,
     * then all frames will be key-frames (kmin value does
     * not matter for these special cases).
     */
    val kmin: Int? = null,
    val kmax: Int? = null,

    /**
     * If true, use mixed compression mode; may choose
     * either lossy and lossless for each frame.
     */
    val allowMixed: Boolean? = null,

    /**
     * If true, print info and warning messages to stderr.
     */
    val verbose: Boolean? = null,

    /**
     * Animation parameters.
     */
    val animParams: WebPMuxAnimParams? = null
)