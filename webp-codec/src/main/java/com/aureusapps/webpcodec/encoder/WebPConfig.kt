package com.aureusapps.webpcodec.encoder

import androidx.annotation.FloatRange
import androidx.annotation.IntDef
import androidx.annotation.IntRange

class WebPConfig(

    /**
     * Lossless encoding (0=lossy(default), 1=lossless).
     */
    @get: Compression
    @setparam:Compression
    var lossless: Int? = null,

    /**
     * between 0 and 100. For lossy, 0 gives the smallest
     * size and 100 the largest. For lossless, this
     * parameter is the amount of effort put into the
     * compression: 0 is the fastest but gives larger
     * files compared to the slowest, but best, 100.
     */
    @FloatRange(from = 0.0, to = 100.0)
    var quality: Float? = null,

    /**
     * quality/speed trade-off (0=fast, 6=slower-better)
     */
    @IntRange(from = 0, to = 6)
    var method: Int? = null,

    /**
     * if non-zero, set the desired target size in bytes.
     * Takes precedence over the 'compression' parameter.
     */
    var targetSize: Int? = null,

    /**
     * if non-zero, specifies the minimal distortion to
     * try to achieve. Takes precedence over target_size.
     */
    var targetPSNR: Float? = null,

    /**
     * maximum number of segments to use, in [1..4]
     */
    @IntRange(from = 1, to = 4)
    var segments: Int? = null,

    /**
     * Spatial Noise Shaping. 0=off, 100=maximum.
     */
    @IntRange(from = 0, to = 100)
    var snsStrength: Int? = null,

    /**
     * range: [0 = off .. 100 = strongest]
     */
    @IntRange(from = 0, to = 100)
    var filterStrength: Int? = null,

    /**
     * range: [0 = off .. 7 = least sharp]
     */
    @IntRange(from = 0, to = 7)
    var filterSharpness: Int? = null,

    /**
     * filtering type: 0 = simple, 1 = strong (only used
     * if filter_strength > 0 or autofilter > 0)
     */
    @get: FilterType
    @setparam:FilterType
    var filterType: Int? = null,

    /**
     * Auto adjust filter's strength [0 = off, 1 = on]
     */
    var autoFilter: Boolean? = null,

    /**
     * Algorithm for encoding the alpha plane (0 = none,
     * 1 = compressed with WebP lossless). Default is 1.
     */
    @get: AlphaCompression
    @setparam:AlphaCompression
    var alphaCompression: Int? = null,

    /**
     * Predictive filtering method for alpha plane.
     * 0: none, 1: fast, 2: best. Default if 1.
     */
    @get: AlphaFiltering
    @setparam:AlphaFiltering
    var alphaFiltering: Int? = null,

    /**
     * Between 0 (smallest size) and 100 (lossless).
     * Default is 100.
     */
    @IntRange(from = 0, to = 100)
    var alphaQuality: Int? = null,

    /**
     * number of entropy-analysis passes (in [1..10]).
     */
    @IntRange(from = 0, to = 10)
    var pass: Int? = null,

    /**
     * if true, export the compressed picture back.
     * In-loop filtering is not applied.
     */
    var showCompressed: Boolean? = null,

    /**
     * preprocessing filter:
     * 0=none, 1=segment-smooth, 2=pseudo-random dithering
     */
    @get: Preprocessing
    @setparam:Preprocessing
    var preprocessing: Int? = null,

    /**
     * log2(number of token partitions) in [0..3]. Default
     * is set to 0 for easier progressive decoding.
     */
    @IntRange(from = 0, to = 3)
    var partitions: Int? = null,

    /**
     * quality degradation allowed to fit the 512k limit
     * on prediction modes coding (0: no degradation,
     * 100: maximum possible degradation).
     */
    @IntRange(from = 0, to = 100)
    var partitionLimit: Int? = null,

    /**
     * If true, compression parameters will be remapped
     * to better match the expected output size from
     * JPEG compression. Generally, the output size will
     * be similar but the degradation will be lower.
     */
    var emulateJPEGSize: Boolean? = null,

    /**
     * If non-zero, try and use multi-threaded encoding.
     */
    var threadLevel: Int? = null,

    /**
     * If set, reduce memory usage (but increase CPU use).
     */
    var lowMemory: Boolean? = null,

    /**
     * Near lossless encoding [0 = max loss .. 100 = off
     * (default)].
     */
    @IntRange(from = 0, to = 100)
    var nearLossless: Int? = null,

    /**
     * if non-zero, preserve the exact RGB values under
     * transparent area. Otherwise, discard this invisible
     * RGB information for better compression. The default
     * value is 0.
     */
    var exact: Boolean? = null,

    /**
     * reserved for future lossless feature
     */
    var useDeltaPalette: Boolean? = null,

    /**
     * if needed, use sharp (and slow) RGB->YUV conversion
     */
    var useSharpYUV: Boolean? = null,

    /**
     * minimum permissible quality factor
     */
    var qmin: Int? = null,

    /**
     * maximum permissible quality factor
     */
    var qmax: Int? = null

) {
    companion object {
        const val COMPRESSION_LOSSY = 0
        const val COMPRESSION_LOSSLESS = 1
        const val FILTER_SIMPLE = 0
        const val FILTER_STRONG = 1
        const val ALPHA_COMPRESSION_NONE = 0
        const val ALPHA_COMPRESSION_WITH_LOSSLESS = 1
        const val ALPHA_FILTERING_NONE = 0
        const val ALPHA_FILTERING_FAST = 1
        const val ALPHA_FILTERING_BEST = 2
        const val PREPROCESSING_NONE = 0
        const val PREPROCESSING_SEGMENT_SMOOTH = 1
        const val PREPROCESSING_RANDOM_DITHERING = 2
        const val WEBP_PRESET_DEFAULT = 0      // default preset
        const val WEBP_PRESET_PICTURE = 1      // digital picture, like portrait, inner shot
        const val WEBP_PRESET_PHOTO = 2        // outdoor photograph, with natural lighting
        const val WEBP_PRESET_DRAWING = 3      // hand or line drawing, with high-contrast details
        const val WEBP_PRESET_ICON = 4         // small-sized colorful images
        const val WEBP_PRESET_TEXT = 5         // text-like
    }

    @Retention(AnnotationRetention.SOURCE)
    @IntDef(COMPRESSION_LOSSY, COMPRESSION_LOSSLESS)
    private annotation class Compression

    @Retention(AnnotationRetention.SOURCE)
    @IntDef(FILTER_SIMPLE, FILTER_STRONG)
    private annotation class FilterType

    @Retention(AnnotationRetention.SOURCE)
    @IntDef(ALPHA_COMPRESSION_NONE, ALPHA_COMPRESSION_WITH_LOSSLESS)
    private annotation class AlphaCompression

    @Retention(AnnotationRetention.SOURCE)
    @IntDef(ALPHA_FILTERING_NONE, ALPHA_FILTERING_FAST, ALPHA_FILTERING_BEST)
    private annotation class AlphaFiltering

    @Retention(AnnotationRetention.SOURCE)
    @IntDef(PREPROCESSING_NONE, PREPROCESSING_SEGMENT_SMOOTH, PREPROCESSING_RANDOM_DITHERING)
    private annotation class Preprocessing

}