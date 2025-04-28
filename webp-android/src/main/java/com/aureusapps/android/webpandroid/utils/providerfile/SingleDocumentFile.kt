package com.aureusapps.android.webpandroid.utils.providerfile

import android.content.Context
import android.net.Uri
import android.provider.DocumentsContract

internal class SingleDocumentFile(
    override val parent: ProviderFile?,
    private val context: Context,
    override val uri: Uri,
) : ProviderFile() {
    override fun createFile(mimeType: String, displayName: String): ProviderFile? {
        throw UnsupportedOperationException()
    }

    override fun createDirectory(displayName: String): ProviderFile? {
        throw UnsupportedOperationException()
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

    override fun listFiles(): List<ProviderFile> {
        throw UnsupportedOperationException()
    }

    override fun renameTo(displayName: String): Boolean {
        throw UnsupportedOperationException()
    }
}
