package com.aureusapps.android.webpandroid.utils.providerfile

import android.annotation.SuppressLint
import android.content.Context
import android.database.Cursor
import android.net.Uri
import android.provider.MediaStore
import com.aureusapps.android.webpandroid.utils.Logger
import okhttp3.internal.closeQuietly

internal object MediaStoreHelper {
    private const val TAG = "MediaStoreHelper"

    fun getName(context: Context, uri: Uri): String? {
        return ProviderHelper.queryForString(context, uri, MediaStore.MediaColumns.DISPLAY_NAME)
    }

    fun getType(context: Context, uri: Uri): String? {
        return ProviderHelper.queryForString(context, uri, MediaStore.MediaColumns.MIME_TYPE)
    }

    fun lastModified(context: Context, uri: Uri): Long {
        return ProviderHelper.queryForLong(context, uri, MediaStore.MediaColumns.DATE_MODIFIED)
    }

    fun length(context: Context, uri: Uri): Long {
        return ProviderHelper.queryForLong(context, uri, MediaStore.MediaColumns.SIZE)
    }

    @SuppressLint("Recycle")
    fun exists(context: Context, uri: Uri): Boolean {
        val resolver = context.contentResolver
        var cursor: Cursor? = null
        return try {
            cursor = resolver.query(uri, arrayOf(MediaStore.MediaColumns._ID), null, null, null)
            cursor != null && cursor.count > 0
        } catch (e: Exception) {
            val message = e.message ?: e::class.simpleName ?: "Unknown error"
            Logger.e(TAG, message)
            false
        } finally {
            cursor?.closeQuietly()
        }
    }
}