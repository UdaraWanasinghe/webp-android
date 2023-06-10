package com.aureusapps.android.webpandroid.encoder

fun interface WebPEncoderProgressListener {
    fun onProgressChanged(progress: Int): Boolean
}