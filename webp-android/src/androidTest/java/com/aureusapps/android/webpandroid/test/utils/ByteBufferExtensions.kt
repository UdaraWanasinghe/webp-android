package com.aureusapps.android.webpandroid.test.utils

import java.nio.ByteBuffer
import java.nio.charset.Charset

/**
 * Returns integer value of the next count bytes and increment the buffer position by count amount.
 *
 * @param count The number of bytes to read from the buffer. Must be less than or equal to 4 as integer can only have 4 bytes.
 * @return Integer value of read bytes.
 */
fun ByteBuffer.nextBytes(count: Int): Int {
    if (count > 4) {
        throw IndexOutOfBoundsException("Out of byte count [$count]")
    }
    var v = 0
    for (i in 0 until count) {
        val u = get().toInt() and 0xFF
        v = v or (u shl (i * 8))
    }
    return v
}

/**
 * Reads integer value from the buffer and increment the buffer position by 4.
 *
 * @return Read integer value.
 */
fun ByteBuffer.nextInt(): Int {
    return int
}

/**
 * Reads the next `size` bytes from the buffer and interprets them as a string using the specified charset and increment the buffer position by [size] amount.
 *
 * @param size The number of bytes to read from the buffer.
 * @param charset The charset to use for decoding the bytes into a string. (Default: US_ASCII)
 * @return The decoded string.
 */
fun ByteBuffer.nextString(size: Int, charset: Charset = Charsets.US_ASCII): String {
    return String(ByteArray(size) { get() }, charset)
}

/**
 * Increment the position of the buffer by `count` amount.
 *
 * @param count The number of bytes to increment.
 */
fun ByteBuffer.skipBytes(count: Int) {
    position(position() + count)
}

/**
 * Writes the specified string into the buffer using the buffer's current position and the specified charset.
 * Advances the position by the number of bytes written.
 *
 * @param value The string to write into the buffer.
 * @return The ByteBuffer instance for method chaining.
 */
fun ByteBuffer.putString(value: String): ByteBuffer {
    put(value.toByteArray(Charsets.US_ASCII))
    return this
}