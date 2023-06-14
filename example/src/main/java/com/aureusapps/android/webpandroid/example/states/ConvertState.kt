package com.aureusapps.android.webpandroid.example.states

import android.net.Uri

internal interface ConvertState {

    interface CancellableState {
        val cancel: () -> Unit
    }

    sealed class ImageToWebP(
        val srcUri: Uri,
        val dstUri: Uri,
        val imageWidth: Int,
        val imageHeight: Int
    ) {

        class OnConvertStarted(
            srcUri: Uri,
            dstUri: Uri,
            imageWidth: Int,
            imageHeight: Int,
            override val cancel: () -> Unit
        ) : ImageToWebP(
            srcUri,
            dstUri,
            imageWidth,
            imageHeight
        ), CancellableState

        class OnConvertProgress(
            parent: ImageToWebP,
            val progress: Int,
            override val cancel: () -> Unit
        ) : ImageToWebP(
            parent.srcUri,
            parent.dstUri,
            parent.imageWidth,
            parent.imageHeight
        ), CancellableState

        class OnConvertFinished(
            parent: ImageToWebP,
        ) : ImageToWebP(
            parent.srcUri,
            parent.dstUri,
            parent.imageWidth,
            parent.imageHeight
        )

        class OnConvertError(
            parent: ImageToWebP,
            val errorMessage: String
        ) : ImageToWebP(
            parent.srcUri,
            parent.dstUri,
            parent.imageWidth,
            parent.imageHeight
        )

    }

    sealed class ImagesToAnimatedWebP(
        val srcUris: List<Uri>,
        val dstUri: Uri,
        val imageWidth: Int,
        val imageHeight: Int
    ) {

        val frameCount get() = srcUris.size

        class OnConvertStarted(
            srcUris: List<Uri>,
            dstUri: Uri,
            imageWidth: Int,
            imageHeight: Int,
            override val cancel: () -> Unit
        ) : ImagesToAnimatedWebP(
            srcUris,
            dstUri,
            imageWidth,
            imageHeight
        ), CancellableState

        class OnConvertProgress(
            parent: ImagesToAnimatedWebP,
            val currentFrame: Int,
            val frameProgress: Int,
            override val cancel: () -> Unit
        ) : ImagesToAnimatedWebP(
            parent.srcUris,
            parent.dstUri,
            parent.imageWidth,
            parent.imageHeight
        ), CancellableState

        class OnConvertFinished(
            parent: ImagesToAnimatedWebP
        ) : ImagesToAnimatedWebP(
            parent.srcUris,
            parent.dstUri,
            parent.imageWidth,
            parent.imageHeight
        )

        class OnConvertError(
            parent: ImagesToAnimatedWebP,
            val errorMessage: String
        ) : ImagesToAnimatedWebP(
            parent.srcUris,
            parent.dstUri,
            parent.imageWidth,
            parent.imageHeight
        )

    }

}