package com.aureusapps.android.webpandroid.extensions

import java.nio.ByteBuffer
import java.nio.charset.Charset

fun ByteBuffer.nextByte(): Byte {
    return get()
}

fun ByteBuffer.nextBytes(count: Int): UInt {
    var v = 0u
    for (i in 0 until count) {
        val u = get().toUInt() and 0xFFu
        v = v or (u shl (i * 8))
    }
    return v
}

fun Byte.getBits(vararg indexes: Int): UInt {
    var v = 0u
    for (i in indexes) {
        v = v or ((1u shl i) and toUInt())
    }
    return v
}

fun ByteBuffer.nextUInt(): UInt {
    return int.toUInt()
}

fun ByteBuffer.nextInt(): Int {
    return int
}

fun UInt.getBits(vararg indexes: Int): UInt {
    var v = 0u
    for (i in indexes) {
        v = v or (1u shl i and this)
    }
    return v
}

fun ByteBuffer.nextString(size: Int, charset: Charset = Charsets.US_ASCII): String {
    return String(ByteArray(size) { get() }, charset)
}

fun ByteBuffer.skipBytes(count: Int) {
    position(position() + count)
}