package com.aureusapps.android.webpandroid.decoder

import android.graphics.Bitmap

/**
 * Configuration data class for the webp decoder.
 * Image file name of the second frame according to below configurations: IMG_0001.png
 *
 * @param namePrefix The prefix to be added to the decoded image file names.
 * @param repeatCharacter The character used for repeating in the file names.
 * @param repeatCharacterCount The number of times the repeat character is repeated in the file names.
 * @param compressFormat The image compression format to be used when saving the decoded image.
 * @param compressQuality The compression quality to be used when saving the decoded image.
 */
data class DecoderConfig(
    val namePrefix: String = "IMG_",
    val repeatCharacter: Char = '0',
    val repeatCharacterCount: Int = 4,
    val compressFormat: Bitmap.CompressFormat = Bitmap.CompressFormat.PNG,
    val compressQuality: Int = 100
)