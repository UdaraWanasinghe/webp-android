package com.aureusapps.android.webpandroid.utils

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri

class BitmapFactoryExtended {

    companion object {

        @JvmStatic
        fun decodeUri(context: Context, uri: Uri): Bitmap {
            val inputStream = context.contentResolver.openInputStream(uri)
            val bitmap = BitmapFactory.decodeStream(inputStream)
            inputStream?.close()
            return bitmap
        }

    }

}