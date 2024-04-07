package com.aureusapps.android.webpandroid.utils

import com.aureusapps.android.webpandroid.CodecResult

object CodecHelper {

    fun resultCodeToCodecResult(resultCode: Int): CodecResult {
        return when (resultCode) {
            0 -> CodecResult.SUCCESS
            else -> throw RuntimeException("Unknown result code")
        }
    }

}