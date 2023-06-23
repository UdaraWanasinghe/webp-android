package com.aureusapps.android.webpandroid.encoder

/**
 * The [WebPPreset] enum class represents different presets for WebP encoding.
 *
 * @param value The integer value associated with each preset.
 */
enum class WebPPreset(val value: Int) {
    WEBP_PRESET_DEFAULT(0),
    WEBP_PRESET_PICTURE(1),
    WEBP_PRESET_PHOTO(2),
    WEBP_PRESET_DRAWING(3),
    WEBP_PRESET_ICON(4),
    WEBP_PRESET_TEXT(5)
}