package com.aureusapps.android.webpandroid.encoder

import androidx.annotation.FloatRange
import androidx.annotation.IntDef
import androidx.annotation.IntRange

/**
 * Configuration options for the WebP encoder.
 *
 * The [WebPConfig] data class provides various options to configure the encoding process
 * for WebP images. These options allow you to control the encoding parameters such as
 * lossless/lossy encoding, quality, compression method, target size, segmentation,
 * filtering, alpha channel encoding, preprocessing, multi-threading, memory usage, and more.
 *
 * @property lossless Enables or disables lossless encoding. Use [WebPConfig.COMPRESSION_LOSSY]
 * for lossy encoding and [WebPConfig.COMPRESSION_LOSSLESS] for lossless encoding.
 * @property quality Specifies the quality factor for lossy encoding. Valid values range from 0 to 100.
 * @property method Specifies the compression method to be used. Valid values range from 0 to 6.
 * @property targetSize Specifies the desired target size in bytes for the output image.
 * @property targetPSNR Specifies the desired target PSNR (Peak Signal-to-Noise Ratio) value for the output image.
 * @property segments Specifies the number of segments to use for multi-threaded encoding. Valid values range from 1 to 4.
 * @property snsStrength Specifies the strength of the spatial noise shaping. Valid values range from 0 to 100.
 * @property filterStrength Specifies the strength of the deblocking filter. Valid values range from 0 to 100.
 * @property filterSharpness Specifies the sharpness of the deblocking filter. Valid values range from 0 to 7.
 * @property filterType Specifies the filtering type. Use [WebPConfig.FILTER_SIMPLE] for simple filtering
 * and [WebPConfig.FILTER_STRONG] for strong filtering.
 * @property autoFilter Controls the automatic filtering strength. If set to true, automatic filtering is enabled.
 * @property alphaCompression Specifies the compression method for the alpha channel. Use [WebPConfig.ALPHA_COMPRESSION_NONE]
 * for no compression and [WebPConfig.ALPHA_COMPRESSION_WITH_LOSSLESS] for lossless compression.
 * @property alphaFiltering Specifies the predictive filtering method for the alpha channel. Use [WebPConfig.ALPHA_FILTERING_NONE]
 * for no filtering, [WebPConfig.ALPHA_FILTERING_FAST] for fast filtering, and [WebPConfig.ALPHA_FILTERING_BEST]
 * for best filtering.
 * @property alphaQuality Specifies the quality factor for lossy alpha encoding. Valid values range from 0 to 100.
 * @property pass Specifies the number of entropy analysis passes. Valid values range from 0 to 10.
 * @property showCompressed If true, export the compressed picture back. In-loop filtering is not applied.
 * @property preprocessing Specifies the preprocessing filter to be applied to the input image. Use [WebPConfig.PREPROCESSING_NONE]
 * for no preprocessing, [WebPConfig.PREPROCESSING_SEGMENT_SMOOTH] for segment smoothing, and
 * [WebPConfig.PREPROCESSING_RANDOM_DITHERING] for pseudo-random dithering.
 * @property partitions log2(number of token partitions) in [0..3]. Default is set to 0 for easier progressive decoding.
 * @property partitionLimit Specifies the maximum number of bytes to use for the internal partition size. Valid values range from 0 to 100.
 * @property emulateJPEGSize If set to true, tries to emulate the compression behavior of JPEG.
 * @property threadLevel If non-zero, specifies the level of effort to use for multi-threaded encoding.
 * @property lowMemory If set to true, reduces memory usage but at the cost of increased CPU usage.
 * @property nearLossless Specifies the level of near lossless encoding. Valid values range from 0 to 100. [0 = max loss .. 100 = off (default)]
 * @property exact If set to true, preserves exact RGB values under transparent areas.
 * @property useDeltaPalette If set to true, enables the use of delta palette encoding.
 * @property useSharpYUV If set to true, uses a sharp RGB to YUV conversion method.
 * @property qmin Specifies the minimum permissible quality factor.
 * @property qmax Specifies the maximum permissible quality factor.
 */
data class WebPConfig(
    @get: Compression
    val lossless: Int? = null,

    @FloatRange(from = 0.0, to = 100.0)
    val quality: Float? = null,

    @IntRange(from = 0, to = 6)
    val method: Int? = null,

    val targetSize: Int? = null,

    val targetPSNR: Float? = null,

    @IntRange(from = 1, to = 4)
    val segments: Int? = null,

    @IntRange(from = 0, to = 100)
    val snsStrength: Int? = null,

    @IntRange(from = 0, to = 100)
    val filterStrength: Int? = null,

    @IntRange(from = 0, to = 7)
    val filterSharpness: Int? = null,

    @get: FilterType
    val filterType: Int? = null,

    val autoFilter: Boolean? = null,

    @get: AlphaCompression
    val alphaCompression: Int? = null,

    @get: AlphaFiltering
    val alphaFiltering: Int? = null,

    @IntRange(from = 0, to = 100)
    val alphaQuality: Int? = null,

    @IntRange(from = 0, to = 10)
    val pass: Int? = null,

    val showCompressed: Boolean? = null,

    @get: Preprocessing
    val preprocessing: Int? = null,

    @IntRange(from = 0, to = 3)
    val partitions: Int? = null,

    @IntRange(from = 0, to = 100)
    val partitionLimit: Int? = null,

    val emulateJPEGSize: Boolean? = null,

    val threadLevel: Int? = null,

    val lowMemory: Boolean? = null,

    @IntRange(from = 0, to = 100)
    val nearLossless: Int? = null,

    val exact: Boolean? = null,

    val useDeltaPalette: Boolean? = null,

    val useSharpYUV: Boolean? = null,

    val qmin: Int? = null,

    val qmax: Int? = null
) {

    companion object {
        // Constants for options with predefined values
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
    }

    // Annotation definitions for restricting the allowed values for properties
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