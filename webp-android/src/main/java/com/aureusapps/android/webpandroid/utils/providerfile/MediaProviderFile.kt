package com.aureusapps.android.webpandroid.utils.providerfile

import android.content.Context
import android.net.Uri

class MediaProviderFile(
    private val context: Context,
    override val uri: Uri,
) : ProviderFile() {
    override val parent: ProviderFile? = null

    override fun createFile(mimeType: String, displayName: String): ProviderFile? {
        throw UnsupportedOperationException()
    }

    override fun createDirectory(displayName: String): ProviderFile? {
        throw UnsupportedOperationException()
    }

    override val name: String?
        get() = MediaStoreHelper.getName(context, uri)

    override val type: String?
        get() = MediaStoreHelper.getType(context, uri)

    override val isDirectory: Boolean = false

    override val isFile: Boolean = true

    override val isVirtual: Boolean = false

    override fun lastModified(): Long {
        return MediaStoreHelper.lastModified(context, uri)
    }

    override fun length(): Long {
        return MediaStoreHelper.length(context, uri)
    }

    override fun canRead(): Boolean {
        return ProviderHelper.canRead(context, uri)
    }

    override fun canWrite(): Boolean {
        return ProviderHelper.canWrite(context, uri)
    }

    override fun delete(): Boolean {
        throw UnsupportedOperationException()
    }

    override fun exists(): Boolean {
        return MediaStoreHelper.exists(context, uri)
    }

    override fun listFiles(): List<ProviderFile> {
        throw UnsupportedOperationException()
    }

    override fun renameTo(displayName: String): Boolean {
        throw UnsupportedOperationException()
    }
}