package com.aureusapps.android.webpandroid.example.states

import android.net.Uri

internal interface ConvertState {

    sealed class ImageToWebP(
        val srcUri: Uri,
        val dstUri: Uri,
        val dstImageWidth: Int,
        val dstImageHeight: Int,
        val cancelCallback: () -> Unit = {}
    ) {

        class OnConvertStarted(
            srcUri: Uri,
            dstUri: Uri,
            dstImageWidth: Int,
            dstImageHeight: Int,
            cancelCallback: () -> Unit
        ) : ImageToWebP(srcUri, dstUri, dstImageWidth, dstImageHeight, cancelCallback)

        class OnConvertProgress private constructor(
            srcUri: Uri,
            dstUri: Uri,
            dstImageWidth: Int,
            dstImageHeight: Int,
            val progress: Int,
            cancelCallback: () -> Unit
        ) : ImageToWebP(srcUri, dstUri, dstImageWidth, dstImageHeight, cancelCallback) {

            companion object {
                fun from(
                    parent: ImageToWebP,
                    progress: Int,
                    cancelCallback: () -> Unit
                ): OnConvertProgress {
                    return OnConvertProgress(
                        parent.srcUri,
                        parent.dstUri,
                        parent.dstImageWidth,
                        parent.dstImageHeight,
                        progress,
                        cancelCallback
                    )
                }
            }

        }

        class OnConvertFinished private constructor(
            srcUri: Uri,
            dstUri: Uri,
            dstImageWidth: Int,
            dstImageHeight: Int
        ) : ImageToWebP(srcUri, dstUri, dstImageWidth, dstImageHeight) {

            companion object {
                fun from(parent: ImageToWebP): OnConvertFinished {
                    return OnConvertFinished(
                        parent.srcUri,
                        parent.dstUri,
                        parent.dstImageWidth,
                        parent.dstImageHeight
                    )
                }
            }

        }

        class OnConvertError private constructor(
            srcUri: Uri,
            dstUri: Uri,
            dstImageWidth: Int,
            dstImageHeight: Int,
            val errorMessage: String
        ) : ImageToWebP(srcUri, dstUri, dstImageWidth, dstImageHeight) {

            companion object {
                fun from(
                    parent: ImageToWebP,
                    errorMessage: String
                ): OnConvertError {
                    return OnConvertError(
                        parent.srcUri,
                        parent.dstUri,
                        parent.dstImageWidth,
                        parent.dstImageHeight,
                        errorMessage
                    )
                }
            }

        }

    }

}