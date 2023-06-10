package com.aureusapps.android.webpandroid.example.actions

import android.net.Uri

// Actions are executed by user.
// For an example: clicking a button.
internal sealed interface UiAction {
    object DeleteCacheAction : UiAction

    sealed interface ImageToWebP : UiAction {
        object OpenDataCollectBottomSheet : ImageToWebP
        object OpenSrcUriPicker : ImageToWebP
        object OpenDstUriPicker : ImageToWebP
        object RequestStartConvert : ImageToWebP
        data class SelectSrcUri(val srcUri: Uri) : ImageToWebP
        data class SelectDstUri(val dstUri: Uri) : ImageToWebP
        data class SelectDstImageWidth(val imageWidth: Int) : ImageToWebP
        data class SelectDstImageHeight(val imageHeight: Int) : ImageToWebP
    }

}