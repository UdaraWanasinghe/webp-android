package com.aureusapps.android.webpandroid.example.states

import android.net.Uri
import com.aureusapps.android.webpandroid.decoder.WebPInfo

internal interface ConvertState {

    interface CancellableState {
        val cancel: () -> Unit
    }

    sealed class ImageToWebP {

        abstract val srcUri: Uri
        abstract val dstUri: Uri
        abstract val imageWidth: Int
        abstract val imageHeight: Int

        data class OnConvertStarted(
            override val srcUri: Uri,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int,
            override val cancel: () -> Unit
        ) : ImageToWebP(), CancellableState

        data class OnConvertProgress(
            override val srcUri: Uri,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int,
            val progress: Int,
            override val cancel: () -> Unit
        ) : ImageToWebP(), CancellableState {

            constructor(
                parent: ImageToWebP,
                progress: Int,
                cancel: () -> Unit
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri,
                imageWidth = parent.imageWidth,
                imageHeight = parent.imageHeight,
                progress = progress,
                cancel = cancel
            )

        }

        data class OnConvertFinished(
            override val srcUri: Uri,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int
        ) : ImageToWebP() {

            constructor(
                parent: ImageToWebP
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri,
                imageWidth = parent.imageWidth,
                imageHeight = parent.imageHeight
            )

        }

        data class OnConvertError(
            override val srcUri: Uri,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int,
            val errorMessage: String
        ) : ImageToWebP() {

            constructor(
                parent: ImageToWebP,
                errorMessage: String
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri,
                imageWidth = parent.imageWidth,
                imageHeight = parent.imageHeight,
                errorMessage = errorMessage,
            )

        }

    }

    sealed class ImagesToAnimatedWebP {

        abstract val srcUris: List<Uri>
        abstract val dstUri: Uri
        abstract val imageWidth: Int
        abstract val imageHeight: Int

        val frameCount get() = srcUris.size

        data class OnConvertStarted(
            override val srcUris: List<Uri>,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int,
            override val cancel: () -> Unit
        ) : ImagesToAnimatedWebP(), CancellableState

        data class OnConvertProgress(
            override val srcUris: List<Uri>,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int,
            val currentFrame: Int,
            val frameProgress: Int,
            override val cancel: () -> Unit
        ) : ImagesToAnimatedWebP(), CancellableState {

            constructor(
                parent: ImagesToAnimatedWebP,
                currentFrame: Int,
                frameProgress: Int,
                cancel: () -> Unit
            ) : this(
                srcUris = parent.srcUris,
                dstUri = parent.dstUri,
                imageWidth = parent.imageWidth,
                imageHeight = parent.imageHeight,
                currentFrame = currentFrame,
                frameProgress = frameProgress,
                cancel = cancel
            )

        }

        data class OnConvertFinished(
            override val srcUris: List<Uri>,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int
        ) : ImagesToAnimatedWebP() {

            constructor(
                parent: ImagesToAnimatedWebP
            ) : this(
                srcUris = parent.srcUris,
                dstUri = parent.dstUri,
                imageWidth = parent.imageWidth,
                imageHeight = parent.imageHeight
            )

        }

        class OnConvertError(
            override val srcUris: List<Uri>,
            override val dstUri: Uri,
            override val imageWidth: Int,
            override val imageHeight: Int,
            val errorMessage: String
        ) : ImagesToAnimatedWebP() {

            constructor(
                parent: ImagesToAnimatedWebP,
                errorMessage: String
            ) : this(
                srcUris = parent.srcUris,
                dstUri = parent.dstUri,
                imageWidth = parent.imageWidth,
                imageHeight = parent.imageHeight,
                errorMessage = errorMessage
            )

        }

    }

    sealed class WebPToImages {

        abstract val srcUri: Uri
        abstract val dstUri: Uri

        data class OnConvertStarted(
            override val srcUri: Uri,
            override val dstUri: Uri,
            override val cancel: () -> Unit
        ) : WebPToImages(), CancellableState

        data class OnReceiveWebPInfo(
            override val srcUri: Uri,
            override val dstUri: Uri,
            val webPInfo: WebPInfo,
            override val cancel: () -> Unit
        ) : WebPToImages(), CancellableState {

            constructor(
                parent: WebPToImages,
                webPInfo: WebPInfo,
                cancel: () -> Unit
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri,
                webPInfo = webPInfo,
                cancel = cancel
            )

        }

        data class OnConvertProgress(
            override val srcUri: Uri,
            override val dstUri: Uri,
            val progress: Int,
            override val cancel: () -> Unit
        ) : WebPToImages(), CancellableState {

            constructor(
                parent: WebPToImages,
                progress: Int,
                cancel: () -> Unit
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri,
                progress = progress,
                cancel = cancel
            )

        }

        data class OnConvertFinished(
            override val srcUri: Uri,
            override val dstUri: Uri,
        ) : WebPToImages() {

            constructor(
                parent: WebPToImages
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri
            )

        }

        data class OnConvertError(
            override val srcUri: Uri,
            override val dstUri: Uri,
            val errorMessage: String
        ) : WebPToImages() {

            constructor(
                parent: WebPToImages,
                errorMessage: String
            ) : this(
                srcUri = parent.srcUri,
                dstUri = parent.dstUri,
                errorMessage = errorMessage
            )

        }

    }

}