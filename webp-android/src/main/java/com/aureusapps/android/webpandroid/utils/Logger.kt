package com.aureusapps.android.webpandroid.utils

import android.util.Log
import com.aureusapps.android.webpandroid.BuildConfig.DEBUG

object Logger {

    private const val TAG = "WEBP_CODEC_"

    fun d(tag: String, msg: String) {
        if (DEBUG) {
            val t = getTag(tag)
            Log.d(t, msg)
        }
    }

    fun w(tag: String, msg: String) {
        if (DEBUG) {
            val t = getTag(tag)
            Log.w(t, msg)
        }
    }

    fun i(tag: String, msg: String) {
        if (DEBUG) {
            val t = getTag(tag)
            Log.i(t, msg)
        }
    }

    fun e(tag: String, msg: String) {
        if (DEBUG) {
            val t = getTag(tag)
            Log.e(t, msg)
        }
    }

    private fun getTag(tag: String): String {
        return "$TAG$tag"
    }

}