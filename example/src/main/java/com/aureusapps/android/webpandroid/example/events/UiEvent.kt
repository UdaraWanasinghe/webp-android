package com.aureusapps.android.webpandroid.example.events

import android.net.Uri

// Events are more like side effects of an action.
// Events are executed as a result of an action.
internal interface UiEvent {

    sealed interface ImageToWebP : UiEvent {

        object OnDataCollectStarted : ImageToWebP

        data class OnSrcUriSelected(
            val srcUri: Uri
        ) : ImageToWebP

        data class OnDstUriSelected(
            val dstUri: Uri
        ) : ImageToWebP

        data class OnDstImageWidthSelected(
            val imageWidth: Int
        ) : ImageToWebP

        data class OnDstImageHeightSelected(
            val imageHeight: Int
        ) : ImageToWebP

        object OnConvertStarted : ImageToWebP

    }

    object ImageToWebPSourceUriSelectEvent : UiEvent
    object ImageToWebPDestinationUriSelectEvent : UiEvent
    object DeleteCacheEvent : UiEvent

}