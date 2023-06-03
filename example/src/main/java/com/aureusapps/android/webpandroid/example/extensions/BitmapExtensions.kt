package com.aureusapps.android.webpandroid.example.extensions

import android.graphics.Bitmap

internal fun Bitmap.copy(): Bitmap {
    return copy(config, isMutable)
}