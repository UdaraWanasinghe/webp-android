package com.aureusapps.android.webpandroid.example.actions

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig

internal sealed interface UiAction {

    data class EncodeAction(
        val width: Int,
        val height: Int,
        val frames: List<Pair<Long, Uri>>,
        val lastTime: Long,
        val encoderOptions: WebPAnimEncoderOptions?,
        val webPConfig: WebPConfig,
        val outputPath: String
    ) : UiAction

    data class ExtractImagesAction(
        val imagePath: String
    ) : UiAction

    object DeleteCacheAction : UiAction

}