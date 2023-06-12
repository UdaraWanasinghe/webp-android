package com.aureusapps.android.webpandroid.utils

import android.content.ContentResolver
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import okhttp3.OkHttpClient
import okhttp3.Request
import java.io.Closeable

class BitmapFactoryExtended {

    companion object {

        @JvmStatic
        fun decodeUri(context: Context, uri: Uri): Bitmap? {
            try {
                var closeable: Closeable? = null
                val inputStream = when (uri.scheme) {
                    ContentResolver.SCHEME_CONTENT,
                    ContentResolver.SCHEME_ANDROID_RESOURCE,
                    ContentResolver.SCHEME_FILE -> {
                        context.contentResolver.openInputStream(uri)
                    }

                    "http",
                    "https" -> {
                        val request = Request.Builder()
                            .url(uri.toString())
                            .build()
                        val client = OkHttpClient
                            .Builder()
                            .build()
                        val response = client.newCall(request)
                            .execute()
                        if (response.code != 200) {
                            return null
                        }
                        val body = response.body
                        if (body == null) {
                            response.close()
                            return null
                        }
                        closeable = response
                        body.byteStream()
                    }

                    else -> {
                        return null
                    }
                }
                return if (inputStream != null) {
                    val bitmap = BitmapFactory.decodeStream(inputStream)
                    inputStream.close()
                    closeable?.close()
                    bitmap
                } else {
                    null
                }

            } catch (e: Exception) {
                return null
            }
        }

    }

}