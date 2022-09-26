package com.aureusapps.webpandroid.decoder

data class WebPAnimDecoderOptions(
    val colorMode: Int? = null,
    val useThreads: Boolean? = null
) {

    companion object {
        const val COLOR_MODE_RGB = 0
        const val COLOR_MODE_RGBA = 1
        const val COLOR_MODE_BGR = 2
        const val COLOR_MODE_BGRA = 3
        const val COLOR_MODE_ARGB = 4
        const val COLOR_MODE_RGBA_4444 = 5
        const val COLOR_MODE_RGB_565 = 6

        // RGB-premultiplied transparent modes (alpha value is preserved)
        const val COLOR_MODE_rgbA = 7
        const val COLOR_MODE_bgrA = 8
        const val COLOR_MODE_Argb = 9
        const val COLOR_MODE_rgbA_4444 = 10

        // YUV modes must come after RGB ones.
        const val COLOR_MODE_YUV = 11
        const val COLOR_MODE_YUVA = 12  // yuv 4:2:0
        const val COLOR_MODE_LAST = 13
    }
}