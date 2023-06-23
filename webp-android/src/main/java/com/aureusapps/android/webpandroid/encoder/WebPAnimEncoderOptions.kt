package com.aureusapps.android.webpandroid.encoder

/**
 * Options for configuring the WebP animation encoder.
 *
 * @param minimizeSize If true, minimizes the output size (slow) and implicitly disables key-frame insertion.
 * @param kmin The minimum distance between consecutive key frames in the output.
 * @param kmax The maximum distance between consecutive key frames in the output.
 * If kmax <= 0, then key-frame insertion is disabled; and if kmax == 1,
 * then all frames will be key-frames (kmin value does not matter for these special cases).
 * The library may insert some key frames as needed to satisfy this criteria.
 * Note that these conditions should hold: kmax > kmin and kmin >= kmax / 2 + 1.
 * @param allowMixed If true, uses mixed compression mode, allowing lossy and lossless frames.
 * @param verbose If true, enables printing info and warning messages to stderr.
 * @param animParams Animation parameters for the WebP animation.
 */
data class WebPAnimEncoderOptions(
    val minimizeSize: Boolean? = null,
    val kmin: Int? = null,
    val kmax: Int? = null,
    val allowMixed: Boolean? = null,
    val verbose: Boolean? = null,
    val animParams: WebPMuxAnimParams? = null
)