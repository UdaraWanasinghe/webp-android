package com.aureusapps.android.webpandroid.extensions

import android.annotation.SuppressLint
import android.content.ContentResolver.SCHEME_ANDROID_RESOURCE
import android.content.ContentResolver.SCHEME_CONTENT
import android.content.ContentResolver.SCHEME_FILE
import android.content.Context
import android.net.Uri
import android.os.Build
import android.webkit.MimeTypeMap
import androidx.annotation.RequiresApi
import androidx.core.net.toFile
import androidx.core.net.toUri
import com.aureusapps.android.webpandroid.utils.providerfile.ProviderFile
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.internal.closeQuietly
import java.io.File
import java.io.InputStream
import java.io.OutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder

enum class UriExtensionErrors(val message: String) {
    PARENT_FILE_NOT_FOUND("Parent file does not exist."),
    FILE_ALREADY_EXIST("File already exist."),
    FAILED_TO_OVERWRITE("Tried to overwrite the file. But failed to delete it."),
    FILE_CREATION_FAILED("Failed to create file."),
    UNSUPPORTED_URI("Given uri is unsupported."),
    EXCEPTION_OCCURRED("Exception occurred.")
}

/**
 * Creates a new file inside the directory given by uri.
 *
 * @param context The Android context.
 * @param fileName The name of the new file.
 *
 * @return Uri of the newly created file or null if the file creation failed.
 */
fun Uri.createFile(
    context: Context,
    fileName: String,
    overwrite: Boolean = false,
    onError: (UriExtensionErrors) -> Unit = {},
): Uri? {
    var fileUri: Uri? = null
    try {
        when (scheme) {
            SCHEME_FILE -> {
                val parentFile = toFile()
                if (parentFile.exists()) {
                    val childFile = File(parentFile, fileName)
                    val createFile = if (childFile.exists()) {
                        if (overwrite) {
                            val stillExists = if (childFile.isDirectory) {
                                !childFile.deleteRecursively()
                            } else {
                                !childFile.delete()
                            }
                            if (stillExists) {
                                onError(UriExtensionErrors.FAILED_TO_OVERWRITE)
                            }
                            !stillExists
                        } else {
                            onError(UriExtensionErrors.FILE_ALREADY_EXIST)
                            false
                        }
                    } else {
                        true
                    }
                    if (createFile) {
                        if (childFile.createNewFile()) {
                            fileUri = childFile.toUri()
                        } else {
                            onError(UriExtensionErrors.FILE_CREATION_FAILED)
                        }
                    }
                } else {
                    onError(UriExtensionErrors.PARENT_FILE_NOT_FOUND)
                }
            }

            SCHEME_CONTENT -> {
                val parentDoc = ProviderFile.fromUri(context, this)
                if (parentDoc != null && parentDoc.exists()) {
                    val existing = parentDoc.findFile(fileName)
                    val createFile = if (existing != null) {
                        if (overwrite) {
                            if (existing.delete()) {
                                true
                            } else {
                                onError(UriExtensionErrors.FAILED_TO_OVERWRITE)
                                false
                            }
                        } else {
                            onError(UriExtensionErrors.FILE_ALREADY_EXIST)
                            false
                        }
                    } else {
                        true
                    }
                    if (createFile) {
                        val extension = fileName.substringAfterLast(".")
                        val mimeType = MimeTypeMap
                            .getSingleton()
                            .getMimeTypeFromExtension(extension)
                            ?: "application/octet-stream"
                        val childDoc = parentDoc.createFile(mimeType, fileName)
                        if (childDoc != null) {
                            fileUri = childDoc.uri
                        } else {
                            onError(UriExtensionErrors.FILE_CREATION_FAILED)
                        }
                    }
                } else {
                    onError(UriExtensionErrors.PARENT_FILE_NOT_FOUND)
                }
            }

            else -> {
                onError(UriExtensionErrors.UNSUPPORTED_URI)
            }
        }
    } catch (_: Exception) {
        onError(UriExtensionErrors.EXCEPTION_OCCURRED)
    }
    return fileUri
}

private fun InputStream.onClose(block: () -> Unit): InputStream {
    return object : InputStream() {
        override fun read(): Int = this@onClose.read()
        override fun read(b: ByteArray?): Int = this@onClose.read(b)
        override fun read(b: ByteArray?, off: Int, len: Int): Int = this@onClose.read(b, off, len)

        @RequiresApi(Build.VERSION_CODES.TIRAMISU)
        override fun readAllBytes(): ByteArray = this@onClose.readAllBytes()

        @RequiresApi(Build.VERSION_CODES.TIRAMISU)
        override fun readNBytes(len: Int): ByteArray = this@onClose.readNBytes(len)

        @RequiresApi(Build.VERSION_CODES.TIRAMISU)
        override fun readNBytes(b: ByteArray?, off: Int, len: Int): Int =
            this@onClose.readNBytes(b, off, len)

        override fun skip(n: Long): Long = this@onClose.skip(n)
        override fun available(): Int = this@onClose.available()
        override fun mark(readlimit: Int) = this@onClose.mark(readlimit)
        override fun reset() = this@onClose.reset()
        override fun markSupported(): Boolean = this@onClose.markSupported()

        @RequiresApi(Build.VERSION_CODES.TIRAMISU)
        override fun transferTo(out: OutputStream?): Long = this@onClose.transferTo(out)

        override fun close() {
            this@onClose.close()
            block()
        }

        override fun equals(other: Any?): Boolean = this@onClose == other
        override fun hashCode(): Int = this@onClose.hashCode()
        override fun toString(): String = this@onClose.toString()
    }
}

/**
 * Opens an input stream for reading the content represented by the Uri.
 *
 * @param context The context used for accessing content resolver.
 *
 * @return An InputStream for reading the content, or null if the content cannot be opened.
 */
@SuppressLint("Recycle")
fun Uri.openInputStream(context: Context): InputStream? {
    var inputStream: InputStream? = null
    try {
        when (scheme) {
            SCHEME_CONTENT,
            SCHEME_FILE,
            SCHEME_ANDROID_RESOURCE,
                -> {
                inputStream = context.contentResolver.openInputStream(this)
            }

            "http",
            "https",
                -> {
                val request = Request.Builder()
                    .url(toString())
                    .build()
                val client = OkHttpClient
                    .Builder()
                    .build()
                val response = client
                    .newCall(request)
                    .execute()
                val body = response.body
                if (body != null) {
                    if (response.code == 200) {
                        // closing input stream will also close the response
                        inputStream = body.byteStream().onClose { response.close() }
                    } else {
                        response.close()
                    }
                }
            }
        }
    } catch (_: Exception) {
    }
    return inputStream
}

/**
 * Reads the contents of the Uri to a byte array.
 *
 * @param context The context used to access the content resolver.
 *
 * @return A byte array containing the contents of the Uri, or null if the operation fails.
 */
fun Uri.readBytes(context: Context): ByteArray? {
    var inputStream: InputStream? = null
    var bytes: ByteArray? = null
    try {
        inputStream = openInputStream(context)
        bytes = inputStream?.readBytes()
    } catch (_: Exception) {
    } finally {
        inputStream?.closeQuietly()
    }
    return bytes
}

/**
 * Reads the contents of the Uri to a ByteBuffer.
 *
 * @param context The context used to access the content resolver.
 *
 * @return A ByteBuffer containing the contents of the Uri, or null if the operation fails.
 */
fun Uri.readToBuffer(context: Context): ByteBuffer? = readBytes(context)
    ?.let { bytes ->
        ByteBuffer
            .allocateDirect(bytes.size)
            .order(ByteOrder.nativeOrder())
            .put(bytes)
            .position(0) as ByteBuffer
    }

/**
 * Finds a file with the given name within the directory represented by the Uri.
 *
 * @param context The context used for accessing content resolver.
 * @param fileName The name of the file to search for.
 * @return The Uri of the found file, or null if the file is not found or an error occurs.
 */
fun Uri.findFile(context: Context, fileName: String): Uri? {
    var uri: Uri? = null
    try {
        when (scheme) {
            SCHEME_FILE -> {
                uri = path
                    ?.let { File(it) }
                    ?.listFiles()
                    ?.firstOrNull { it.name == fileName }
                    ?.toUri()
            }

            SCHEME_CONTENT -> {
                uri = ProviderFile.fromUri(context, this)
                    ?.findFile(fileName)
                    ?.uri
            }
        }
    } catch (_: Exception) {
    }
    return uri
}
