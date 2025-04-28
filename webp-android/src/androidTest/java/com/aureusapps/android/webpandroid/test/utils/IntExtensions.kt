package com.aureusapps.android.webpandroid.test.utils

/**
 * Retrieves the specified bits from the integer.
 *
 * @param indexes The indexes of the bits to retrieve.
 * @return An integer representing the specified bits.
 * @throws IndexOutOfBoundsException if any of the specified indexes are out of range (0-31).
 */
fun Int.getBits(vararg indexes: Int): Int {
    var v = 0
    for (i in indexes) {
        if (i < 0 || i > 31) {
            throw IndexOutOfBoundsException("Out of integer range [$i]")
        }
        v = v or ((1 shl i) and this)
    }
    return v
}
