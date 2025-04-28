package com.aureusapps.android.webpandroid.utils.providerfile

import android.annotation.SuppressLint
import android.content.Context
import android.database.Cursor
import android.net.Uri
import android.provider.DocumentsContract
import com.aureusapps.android.webpandroid.utils.Logger
import okhttp3.internal.closeQuietly

internal class TreeDocumentFile(
    override val parent: ProviderFile?,
    private val context: Context,
    override var uri: Uri,
) : ProviderFile() {
    override fun createFile(mimeType: String, displayName: String): ProviderFile? {
        val result = createFile(context, uri, mimeType, displayName)
        return if (result != null) TreeDocumentFile(this, context, result) else null
    }

    override fun createDirectory(displayName: String): ProviderFile? {
        val result = createFile(
            context, uri, DocumentsContract.Document.MIME_TYPE_DIR, displayName
        )
        return if (result != null) TreeDocumentFile(this, context, result) else null
    }

    override val name: String?
        get() = DocumentsProviderHelper.getName(context, uri)

    override val type: String?
        get() = DocumentsProviderHelper.getType(context, uri)

    override val isDirectory: Boolean
        get() = DocumentsProviderHelper.isDirectory(context, uri)

    override val isFile: Boolean
        get() = DocumentsProviderHelper.isFile(context, uri)

    override val isVirtual: Boolean
        get() = DocumentsProviderHelper.isVirtual(context, uri)

    override fun lastModified(): Long {
        return DocumentsProviderHelper.lastModified(context, uri)
    }

    override fun length(): Long {
        return DocumentsProviderHelper.length(context, uri)
    }

    override fun canRead(): Boolean {
        return DocumentsProviderHelper.canRead(context, uri)
    }

    override fun canWrite(): Boolean {
        return DocumentsProviderHelper.canWrite(context, uri)
    }

    override fun delete(): Boolean {
        return try {
            DocumentsContract.deleteDocument(context.contentResolver, uri)
        } catch (_: Exception) {
            false
        }
    }

    override fun exists(): Boolean {
        return DocumentsProviderHelper.exists(context, uri)
    }

    @SuppressLint("Recycle")
    override fun listFiles(): List<ProviderFile> {
        val resolver = context.contentResolver
        val childrenUri = DocumentsContract.buildChildDocumentsUriUsingTree(
            uri,
            DocumentsContract.getDocumentId(uri)
        )
        val results = ArrayList<TreeDocumentFile>()
        var c: Cursor? = null
        try {
            c = resolver.query(
                childrenUri,
                arrayOf(DocumentsContract.Document.COLUMN_DOCUMENT_ID),
                null,
                null,
                null
            )
            if (c != null) {
                while (c.moveToNext()) {
                    val documentId = c.getString(0)
                    val documentUri = DocumentsContract.buildDocumentUriUsingTree(uri, documentId)
                    results.add(
                        TreeDocumentFile(this, context, documentUri)
                    )
                }
            }
        } catch (e: Exception) {
            Logger.w(TAG, "Failed query: $e")
        } finally {
            c?.closeQuietly()
        }
        return results
    }

    override fun renameTo(displayName: String): Boolean {
        return try {
            val result = DocumentsContract.renameDocument(
                context.contentResolver,
                uri,
                displayName
            )
            if (result != null) {
                uri = result
                true
            } else {
                false
            }
        } catch (_: Exception) {
            false
        }
    }

    companion object {
        private fun createFile(
            context: Context,
            uri: Uri,
            mimeType: String,
            displayName: String,
        ): Uri? {
            return try {
                DocumentsContract.createDocument(
                    context.contentResolver,
                    uri,
                    mimeType,
                    displayName
                )
            } catch (_: Exception) {
                null
            }
        }
    }
}
