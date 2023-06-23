package com.aureusapps.android.webpandroid.encoder

/**
 * The [WebPEncoderProgressListener] interface defines a callback for receiving progress updates during the WebP encoding process.
 */
fun interface WebPEncoderProgressListener {

    /**
     * This function is called when the encoding progress changes.
     *
     * @param progress The progress value indicating the current state of the encoding process.
     *
     * @return `true` to continue the encoding process, `false` to cancel it.
     */
    fun onProgressChanged(progress: Int): Boolean

}
