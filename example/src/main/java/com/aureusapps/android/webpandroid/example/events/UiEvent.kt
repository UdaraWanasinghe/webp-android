package com.aureusapps.android.webpandroid.example.events

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPPreset

// Events are more like side effects of an action.
// Events are executed as a result of an action.
internal interface UiEvent {

    sealed class ImageToWebP : UiEvent {

        open val actionTag: Any? = null

        data class OnDataCollectStarted(
            val srcUri: Uri,
            val dstUri: Uri,
            val imageWidth: Int,
            val imageHeight: Int,
            val convertQuality: Float,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnSrcUriSelected(
            val srcUri: Uri,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnDstUriSelected(
            val dstUri: Uri,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnConvertQualitySelected(
            val quality: Float,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnImageWidthSelected(
            val imageWidth: Int,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnImageHeightSelected(
            val imageHeight: Int,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnConvertPresetSelected(
            val preset: WebPPreset,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnConvertStarted(
            override val actionTag: Any? = null
        ) : ImageToWebP()

    }

    sealed class ImagesToAnimatedWebP : UiEvent {

        open val actionTag: Any? = null

        data class OnDataCollectStarted(
            val srcUris: List<Uri>,
            val dstUri: Uri,
            val convertQuality: Float,
            val imageWidth: Int,
            val imageHeight: Int,
            val webPPreset: WebPPreset?,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnSrcUrisSelected(
            val srcUris: List<Uri>,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnDstUriSelected(
            val dstUri: Uri,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnConvertQualitySelected(
            val quality: Float,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnImageWidthSelected(
            val imageWidth: Int,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnImageHeightSelected(
            val imageHeight: Int,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnWebPPresetSelected(
            val preset: WebPPreset,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnConvertStarted(
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

    }

}