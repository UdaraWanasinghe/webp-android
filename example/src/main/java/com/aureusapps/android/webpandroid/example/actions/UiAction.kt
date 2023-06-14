package com.aureusapps.android.webpandroid.example.actions

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPPreset

// Actions are executed by user.
// For an example: clicking a button.
internal sealed interface UiAction {

    object DeleteCacheAction : UiAction

    sealed class ImageToWebP : UiAction {

        open val tag: Any? = null

        data class OpenDataCollectBottomSheet(
            val defaultConvertQuality: Float = 70f,
            val defaultImageWidth: Int = 512,
            val defaultImageHeight: Int = 512,
            val defaultPreset: WebPPreset? = null,
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
            val quality: Float,
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
            val preset: WebPPreset,
            override val tag: Any? = null
        ) : ImageToWebP()
    }

    sealed class ImagesToAnimatedWebP : UiAction {

        open val tag: Any? = null

        data class OpenDataCollectBottomSheet(
            val defaultConvertQuality: Float = 70f,
            val defaultImageWidth: Int = 512,
            val defaultImageHeight: Int = 512,
            val defaultPreset: WebPPreset? = null,
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
            val quality: Float,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()

        data class SelectFrameDuration(
            val duration: Int,
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
            val preset: WebPPreset,
            override val tag: Any? = null
        ) : ImagesToAnimatedWebP()
    }

}