package com.aureusapps.android.webpandroid.example.actions

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPPreset

// Actions are executed by user.
// For an example: clicking a button.
internal sealed interface UiAction {

    sealed class ImageToWebP : UiAction {

        abstract val tag: Any?

        data class OpenDataCollectBottomSheet(
            override val tag: Any? = null
        ) : ImageToWebP()

        data class StartDataCollect(
            val defaultSrcUri: Uri = Uri.EMPTY,
            val defaultDstUri: Uri = Uri.EMPTY,
            val defaultConvertQuality: Float = 70f,
            val defaultImageWidth: Int = -1,
            val defaultImageHeight: Int = -1,
            val defaultWebPPreset: WebPPreset? = WebPPreset.WEBP_PRESET_DEFAULT,
            val startConvert: Boolean = false,
            override val tag: Any? = null
        ) : ImageToWebP()

        data class OpenSrcUriPicker(
            override val tag: Any? = null
        ) : ImageToWebP()

        data class OpenDstUriPicker(
            override val tag: Any? = null
        ) : ImageToWebP()

        data class RequestStartConvert(
            override val tag: Any? = null
        ) : ImageToWebP()

        data class SelectSrcUri(
            val srcUri: Uri,
            override val tag: Any? = null
        ) : ImageToWebP()

        data class SelectDstUri(
            val dstUri: Uri,
            override val tag: Any? = null
        ) : ImageToWebP()

        data class SelectConvertQuality(
            val convertQuality: Float,
            override val tag: Any? = null
        ) : ImageToWebP()

        data class SelectImageWidth(
            val imageWidth: Int,
            override val tag: Any? = null
        ) : ImageToWebP()

        data class SelectImageHeight(
            val imageHeight: Int,
            override val tag: Any? = null
        ) : ImageToWebP()

        data class SelectWebPPreset(
            val webPPreset: WebPPreset,
            override val tag: Any? = null
        ) : ImageToWebP()
    }

    sealed class ImagesToAnimatedWebP : UiAction {

        abstract val tag: Any?

        data class OpenDataCollectBottomSheet(
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class StartDataCollect(
            val defaultSrcUris: List<Uri> = emptyList(),
            val defaultDstUri: Uri = Uri.EMPTY,
            val defaultFrameDuration: Int = 1000,
            val defaultConvertQuality: Float = 70f,
            val defaultImageWidth: Int = -1,
            val defaultImageHeight: Int = -1,
            val defaultWebPPreset: WebPPreset? = WebPPreset.WEBP_PRESET_DEFAULT,
            val startConvert: Boolean = false,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OpenSrcUrisPicker(
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class OpenDstUriPicker(
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class RequestStartConvert(
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectSrcUris(
            val srcUris: List<Uri>,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectDstUri(
            val dstUri: Uri,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectConvertQuality(
            val convertQuality: Float,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectFrameDuration(
            val frameDuration: Int,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectImageWidth(
            val imageWidth: Int,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectImageHeight(
            val imageHeight: Int,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectWebPPreset(
            val webPPreset: WebPPreset,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()
    }

    sealed class WebPToImages : UiAction {

        abstract val tag: Any?

        data class OpenDataCollectBottomSheet(
            override val tag: Any? = null
        ) : WebPToImages()

        data class StartDataCollect(
            val defaultSrcUri: Uri = Uri.EMPTY,
            val defaultDstUri: Uri = Uri.EMPTY,
            val startConvert: Boolean = false,
            override val tag: Any? = null
        ) : WebPToImages()

        data class OpenSrcUriPicker(
            override val tag: Any? = null
        ) : WebPToImages()

        data class SelectSrcUri(
            val srcUri: Uri,
            override val tag: Any? = null
        ) : WebPToImages()

        data class OpenDstUriPicker(
            override val tag: Any? = null
        ) : WebPToImages()

        data class SelectDstUri(
            val dstUri: Uri,
            override val tag: Any? = null
        ) : WebPToImages()

        data class RequestStartConvert(
            override val tag: Any? = null
        ) : WebPToImages()

    }

}