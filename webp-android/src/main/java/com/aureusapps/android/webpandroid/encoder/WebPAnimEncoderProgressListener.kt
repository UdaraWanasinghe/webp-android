package com.aureusapps.android.webpandroid.encoder

/**
 * A functional interface for listening to the encoding progress of the WebP animation.
 */
fun interface WebPAnimEncoderProgressListener {

    /**
     * This function is called when the encoding progress changes for each frame.
     * If you want to cancel the encoding process, you can return false from this function.
     *
     * @param currentFrame The index of the frame currently being encoded, starting from zero.
     * @param frameProgress The percentage of the current frame that has been encoded.
     *
     * @return Return false to cancel the encoding process, or true to continue the encoding.
     */
    fun onProgressChanged(currentFrame: Int, frameProgress: Int): Boolean
    
}