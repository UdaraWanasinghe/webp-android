package com.aureusapps.android.webpandroid.example.data

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPPreset

internal sealed interface ConvertData {

    /**
     * Data required to convert an image to a static webp image.
     */
    data class ImageToWebP(
        val srcUri: Uri = Uri.EMPTY,
        val dstUri: Uri = Uri.EMPTY,
        val imageWidth: Int = 0,
        val imageHeight: Int = 0,
        val convertQuality: Float = 0f,
        val webPPreset: WebPPreset? = null,
        val startConvert: Boolean = false
    )

    /**
     * Data required to convert a list of images to an animated webp image.
     */
    data class ImagesToAnimatedWebP(
        val srcUris: List<Uri> = emptyList(), // Image uri and timestamp pair list.
        val dstUri: Uri = Uri.EMPTY,
        val frameDuration: Int = 0,
        val imageWidth: Int = 0,
        val imageHeight: Int = 0,
        val convertQuality: Float = 0f,
        val webPPreset: WebPPreset? = null,
        val startConvert: Boolean = false
    )

    /**
     * Data required to extract images from a webp image.
     */
    data class WebPToImages(
        val srcUri: Uri = Uri.EMPTY,
        val dstUri: Uri = Uri.EMPTY,
        val startConvert: Boolean = false
    )

}