/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.aureusapps.android.webpandroid.utils.providerfile

import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.database.Cursor
import android.net.Uri
import android.provider.DocumentsContract
import android.text.TextUtils
import androidx.core.provider.DocumentsContractCompat.DocumentCompat
import com.aureusapps.android.webpandroid.utils.Logger

internal object DocumentsProviderHelper {
    private const val TAG = "DocumentFile"

    @JvmStatic
    fun isVirtual(context: Context, self: Uri): Boolean {
        return if (!DocumentsContract.isDocumentUri(context, self)) {
            false
        } else getFlags(
            context,
            self
        ) and DocumentCompat.FLAG_VIRTUAL_DOCUMENT.toLong() != 0L
    }

    @JvmStatic
    fun getName(context: Context, self: Uri): String? {
        return queryForString(context, self, DocumentsContract.Document.COLUMN_DISPLAY_NAME, null)
    }

    private fun getRawType(context: Context, self: Uri): String? {
        return queryForString(context, self, DocumentsContract.Document.COLUMN_MIME_TYPE, null)
    }

    @JvmStatic
    fun getType(context: Context, self: Uri): String? {
        val rawType = getRawType(context, self)
        return if (DocumentsContract.Document.MIME_TYPE_DIR == rawType) {
            null
        } else {
            rawType
        }
    }

    private fun getFlags(context: Context, self: Uri): Long {
        return queryForLong(context, self, DocumentsContract.Document.COLUMN_FLAGS, 0)
    }

    @JvmStatic
    fun isDirectory(context: Context, self: Uri): Boolean {
        return DocumentsContract.Document.MIME_TYPE_DIR == getRawType(context, self)
    }

    @JvmStatic
    fun isFile(context: Context, self: Uri): Boolean {
        val type = getRawType(context, self)
        return !(DocumentsContract.Document.MIME_TYPE_DIR == type || TextUtils.isEmpty(type))
    }

    @JvmStatic
    fun lastModified(context: Context, self: Uri): Long {
        return queryForLong(context, self, DocumentsContract.Document.COLUMN_LAST_MODIFIED, 0)
    }

    @JvmStatic
    fun length(context: Context, self: Uri): Long {
        return queryForLong(context, self, DocumentsContract.Document.COLUMN_SIZE, 0)
    }

    @JvmStatic
    fun canRead(context: Context, self: Uri): Boolean {
        // Ignore if grant doesn't allow read
        if (context.checkCallingOrSelfUriPermission(self, Intent.FLAG_GRANT_READ_URI_PERMISSION)
            != PackageManager.PERMISSION_GRANTED
        ) {
            return false
        }

        // Ignore documents without MIME
        return !TextUtils.isEmpty(getRawType(context, self))
    }

    @JvmStatic
    fun canWrite(context: Context, self: Uri): Boolean {
        // Ignore if grant doesn't allow write
        if (context.checkCallingOrSelfUriPermission(self, Intent.FLAG_GRANT_WRITE_URI_PERMISSION)
            != PackageManager.PERMISSION_GRANTED
        ) {
            return false
        }
        val type = getRawType(context, self)
        val flags = queryForInt(context, self, DocumentsContract.Document.COLUMN_FLAGS, 0)

        // Ignore documents without MIME
        if (TextUtils.isEmpty(type)) {
            return false
        }

        // Deletable documents considered writable
        if (flags and DocumentsContract.Document.FLAG_SUPPORTS_DELETE != 0) {
            return true
        }
        if (DocumentsContract.Document.MIME_TYPE_DIR == type && flags and DocumentsContract.Document.FLAG_DIR_SUPPORTS_CREATE != 0) {
            // Directories that allow create considered writable
            return true
        } else if (!TextUtils.isEmpty(type)
            && flags and DocumentsContract.Document.FLAG_SUPPORTS_WRITE != 0
        ) {
            // Writable normal files considered writable
            return true
        }
        return false
    }

    @JvmStatic
    fun exists(context: Context, self: Uri): Boolean {
        val resolver = context.contentResolver
        var c: Cursor? = null
        return try {
            c = resolver.query(
                self,
                arrayOf(
                    DocumentsContract.Document.COLUMN_DOCUMENT_ID
                ),
                null,
                null,
                null
            )
            (c?.count ?: 0) > 0
        } catch (e: Exception) {
            Logger.w(TAG, "Failed query: $e")
            false
        } finally {
            closeQuietly(c)
        }
    }

    @Suppress("SameParameterValue")
    private fun queryForString(
        context: Context,
        self: Uri,
        column: String,
        defaultValue: String?,
    ): String? {
        val resolver = context.contentResolver
        var c: Cursor? = null
        return try {
            c = resolver.query(self, arrayOf(column), null, null, null)
            if (c != null && c.moveToFirst() && !c.isNull(0)) {
                c.getString(0)
            } else {
                defaultValue
            }
        } catch (e: Exception) {
            Logger.w(TAG, "Failed query: $e")
            defaultValue
        } finally {
            closeQuietly(c)
        }
    }

    @Suppress("SameParameterValue")
    private fun queryForInt(
        context: Context,
        self: Uri,
        column: String,
        defaultValue: Int,
    ): Int {
        return queryForLong(context, self, column, defaultValue.toLong()).toInt()
    }

    private fun queryForLong(
        context: Context,
        self: Uri,
        column: String,
        defaultValue: Long,
    ): Long {
        val resolver = context.contentResolver
        var c: Cursor? = null
        return try {
            c = resolver.query(self, arrayOf(column), null, null, null)
            if (c!!.moveToFirst() && !c.isNull(0)) {
                c.getLong(0)
            } else {
                defaultValue
            }
        } catch (e: Exception) {
            Logger.w(TAG, "Failed query: $e")
            defaultValue
        } finally {
            closeQuietly(c)
        }
    }

    private fun closeQuietly(closeable: AutoCloseable?) {
        if (closeable != null) {
            try {
                closeable.close()
            } catch (rethrown: RuntimeException) {
                throw rethrown
            } catch (_: Exception) {
            }
        }
    }
}
