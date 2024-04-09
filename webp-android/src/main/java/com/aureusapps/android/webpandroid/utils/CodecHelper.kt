package com.aureusapps.android.webpandroid.utils

import com.aureusapps.android.webpandroid.CodecResult

object CodecHelper {

    fun resultCodeToCodecResult(resultCode: Int): CodecResult {
        return CodecResult.values()[resultCode]
    }

}