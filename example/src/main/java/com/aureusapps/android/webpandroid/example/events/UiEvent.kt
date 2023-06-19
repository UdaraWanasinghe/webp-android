package com.aureusapps.android.webpandroid.example.events

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPPreset

// Events are more like side effects of an action.
// Events are executed as a result of an action.
internal interface UiEvent {

    sealed class ImageToWebP : UiEvent {

        abstract val actionTag: Any?

        data class OnOpenDataCollectBottomSheet(
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnDataCollectStarted(
            val srcUri: Uri,
            val dstUri: Uri,
            val imageWidth: Int,
            val imageHeight: Int,
            val convertQuality: Float,
            val webPPreset: WebPPreset?,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnOpenSrcUriPicker(
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnSrcUriSelected(
            val srcUri: Uri,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnOpenDstUriPicker(
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnDstUriSelected(
            val dstUri: Uri,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnConvertQualitySelected(
            val convertQuality: Float,
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
            val webPPreset: WebPPreset,
            override val actionTag: Any? = null
        ) : ImageToWebP()

        data class OnConvertStarted(
            override val actionTag: Any? = null
        ) : ImageToWebP()

    }

    sealed class ImagesToAnimatedWebP : UiEvent {

        abstract val actionTag: Any?

        data class OnOpenDataCollectBottomSheet(
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnDataCollectStarted(
            val srcUris: List<Uri>,
            val dstUri: Uri,
            val frameDuration: Int,
            val convertQuality: Float,
            val imageWidth: Int,
            val imageHeight: Int,
            val webPPreset: WebPPreset?,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnOpenSrcUrisPicker(
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnSrcUrisSelected(
            val srcUris: List<Uri>,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnOpenDstUriPicker(
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnDstUriSelected(
            val dstUri: Uri,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnFrameDurationSelected(
            val frameDuration: Int,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnConvertQualitySelected(
            val convertQuality: Float,
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
            val webPPreset: WebPPreset,
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OnConvertStarted(
            override val actionTag: Any? = null
        ) : ImagesToAnimatedWebP()

    }

    sealed class WebPToImages : UiEvent {

        abstract val actionTag: Any?

        data class OnOpenDataCollectBottomSheet(
            override val actionTag: Any? = null
        ) : WebPToImages()

        data class OnDataCollectStarted(
            val srcUri: Uri,
            val dstUri: Uri,
            override val actionTag: Any? = null
        ) : WebPToImages()

        data class OnOpenSrcUriPicker(
            override val actionTag: Any? = null
        ) : WebPToImages()

        data class OnSrcUriSelected(
            val srcUri: Uri,
            override val actionTag: Any? = null
        ) : WebPToImages()

        data class OnOpenDstUriPicker(
            override val actionTag: Any? = null
        ) : WebPToImages()

        data class OnDstUriSelected(
            val dstUri: Uri,
            override val actionTag: Any? = null
        ) : WebPToImages()

        data class OnConvertStarted(
            override val actionTag: Any? = null
        ) : WebPToImages()

    }
}