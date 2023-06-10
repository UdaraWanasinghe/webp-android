package com.aureusapps.android.webpandroid.example.data

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPPreset

internal sealed interface ConvertData {

    /**
     * Data required to convert an image to a static webp image.
     */
    data class ImageToWebP(
        val srcUri: Uri = Uri.EMPTY,
        val dstUri: Uri = Uri.EMPTY,
        val dstImageWidth: Int = 0,
        val dstImageHeight: Int = 0,
        val webPPreset: WebPPreset? = null,
        val webPConfig: WebPConfig = WebPConfig(),
        val startConvert: Boolean = false
    )

    /**
     * Data required to convert a list of images to an animated webp image.
     */
    data class ImagesToAnimatedWebP(
        val srcUris: List<Pair<Uri, Long>>? = null, // Image uri and timestamp pair list.
        val dstUri: Uri? = null,
        val dstImageWidth: Int? = null,
        val dstImageHeight: Int? = null,
        val endTimestamp: Long? = null,
        val webPAnimEncoderOptions: WebPAnimEncoderOptions = WebPAnimEncoderOptions(),
        val webPConfig: WebPConfig = WebPConfig(),
        val webPPreset: WebPPreset? = null
    )

    /**
     * Data required to extract images from a webp image.
     */
    data class WebPToImages(
        val srcUri: Uri? = null,
        val dstUri: Uri? = null
    )

}