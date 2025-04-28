package com.aureusapps.android.webpandroid.utils.providerfile

import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.database.Cursor
import android.net.Uri
import com.aureusapps.android.webpandroid.utils.Logger
import okhttp3.internal.closeQuietly

internal object ProviderHelper {
    private const val TAG = "ProviderHelper"

    @SuppressLint("Recycle")
    fun queryForString(
        context: Context,
        uri: Uri,
        column: String,
        defaultValue: String? = null,
    ): String? {
        val resolver = context.contentResolver
        var cursor: Cursor? = null
        return try {
            cursor = resolver.query(uri, arrayOf(column), null, null, null)
            if (cursor?.moveToFirst() == true) {
                cursor.getString(0) ?: defaultValue
            } else {
                defaultValue
            }
        } catch (e: Exception) {
            val message = e.message ?: e::class.simpleName ?: "Unknown error"
            Logger.e(TAG, message)
            defaultValue
        } finally {
            cursor?.closeQuietly()
        }
    }

    @SuppressLint("Recycle")
    fun queryForLong(
        context: Context,
        uri: Uri,
        column: String,
        defaultValue: Long = 0,
    ): Long {
        val resolver = context.contentResolver
        var cursor: Cursor? = null
        return try {
            cursor = resolver.query(uri, arrayOf(column), null, null, null)
            if (cursor?.moveToFirst() == true) {
                cursor.getLong(0)
            } else {
                defaultValue
            }
        } catch (e: Exception) {
            val message = e.message ?: e::class.simpleName ?: "Unknown error"
            Logger.e(TAG, message)
            defaultValue
        } finally {
            cursor?.closeQuietly()
        }
    }

    fun canRead(context: Context, uri: Uri): Boolean {
        return context.checkCallingOrSelfUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION) == PackageManager.PERMISSION_GRANTED
    }

    fun canWrite(context: Context, uri: Uri): Boolean {
        return context.checkCallingOrSelfUriPermission(uri, Intent.FLAG_GRANT_WRITE_URI_PERMISSION) == PackageManager.PERMISSION_GRANTED
    }
}