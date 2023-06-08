package com.aureusapps.android.webpandroid.example.actions

import android.net.Uri
import com.aureusapps.android.webpandroid.encoder.WebPAnimEncoderOptions
import com.aureusapps.android.webpandroid.encoder.WebPConfig
import com.aureusapps.android.webpandroid.encoder.WebPPreset

internal sealed interface UiAction {

    data class ConvertBitmapToAnimatedWebPAction(
        val width: Int,
        val height: Int,
        val frames: List<Pair<Long, Uri>>,
        val outputPath: String,
        val lastTime: Long,
        val encoderOptions: WebPAnimEncoderOptions?,
        val webPConfig: WebPConfig,
        val webPPreset: WebPPreset? = null
    ) : UiAction

    data class ConvertWebPToBitmapAction(
        val sourceUri: Uri
    ) : UiAction

    object DeleteCacheAction : UiAction

    data class ConvertBitmapToWebPAction(
        val width: Int,
        val height: Int,
        val sourceUri: Uri,
        val outputPath: String,
        val webPConfig: WebPConfig,
        val webPPreset: WebPPreset? = null
    ) : UiAction

}