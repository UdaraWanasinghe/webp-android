package com.aureusapps.android.webpandroid.decoder

/**
 * The [WebPInfo] data class represents information about a WebP image.
 *
 * @param width The width of the WebP image in pixels.
 * @param height The height of the WebP image in pixels.
 * @param hasAlpha Specifies whether the WebP image has an alpha channel.
 * @param hasAnimation Specifies whether the WebP image contains animation.
 * @param bgColor The background color of the WebP animation canvas (only available if the image has animation).
 * @param frameCount The number of frames in the WebP animation (only available if the image has animation).
 * @param loopCount The number of times the WebP animation should be looped (only available if the image has animation).
 */
data class WebPInfo(
    val width: Int,
    val height: Int,
    val hasAlpha: Boolean,
    val hasAnimation: Boolean,

    // extended info: only if has animation
    val bgColor: Int = 0,
    val frameCount: Int = 0,
    val loopCount: Int = 0
)
