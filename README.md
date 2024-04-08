# webp-android

[![Publish](https://github.com/UdaraWanasinghe/webp-android/actions/workflows/publish.yml/badge.svg)](https://github.com/UdaraWanasinghe/webp-android/actions/workflows/publish.yml)
[![Maven Central](https://img.shields.io/maven-central/v/com.aureusapps.android/webp-android)](https://central.sonatype.com/artifact/com.aureusapps.android/webp-android/1.0.6)
[![javadoc](https://javadoc.io/badge2/com.aureusapps.android/webp-android/1.0.6/javadoc.svg)](https://javadoc.io/doc/com.aureusapps.android/webp-android/1.0.6)

## What's Inside

* Encode a series of android bitmap images to a static or an animated WebP image.
* Extract bitmap images from an animated WebP image.

## Using

### Import the library into your project.

```groovy
// module level build.gradle.kts
dependencies {
    implementation "com.aureusapps.android:webp-android:1.0.6"
}
```

### Encoding a Bitmap with WebPEncoder

```kotlin
// Create encoder instance
val webPEncoder = WebPEncoder(context, width, height)

// Configure the encoder
webPEncoder.configure(
    config = WebPConfig(
        lossless = WebPConfig.COMPRESSION_LOSSLESS,
        quality = 75f
    ),
    preset = WebPPreset.WEBP_PRESET_PICTURE
)

// Add progress listener if desired
webPEncoder.addProgressListener {
    // Handle progress updates
    true // Return true to continue encoding, false to cancel
}

// Encode frame
webPEncoder.encode(srcBitmap, dstUri)

// Release resources
webPEncoder.release()
```

### Encoding a list of Bitmaps with WebPAnimEncoder

```kotlin
val webPAnimEncoder = WebPAnimEncoder(
    context = context,
    width = width,
    height = height,
    options = WebPAnimEncoderOptions(
        minimizeSize = true,
        animParams = WebPMuxAnimParams(
            backgroundColor = Color.GREEN,
            loopCount = 3
        )
    )
)
webPAnimEncoder.configure(
    config = WebPConfig(
        lossless = WebPConfig.COMPRESSION_LOSSLESS,
        quality = 75f
    ),
    preset = WebPPreset.WEBP_PRESET_PICTURE
)

// Add progress listener if desired
webPAnimEncoder.addProgressListener {
    // Handle progress updates
    true // Return true to continue encoding, false to cancel
}

// Add frames to the animation
webPAnimEncoder.addFrame(timestamp, srcBitmap)
webPAnimEncoder.addFrame(timestamp, srcUri)

// Assemble the animation
webPAnimEncoder.assemble(timestamp, dstUri)

// Release resources
webPAnimEncoder.release()
```

### Decoding WebP Image with WebPDecoder

```kotlin
// Create decoder instance
val webPDecoder = WebPDecoder(context)

// Set data source
webPDecoder.setDataSource(context, srcUri)

// Add decode listener to receive decoding events
webPDecoder.addDecodeListener(
    object : WebPDecodeListener {
        override fun onInfoDecoded(info: WebPInfo) {
            // Handle image information
        }

        override fun onFrameDecoded(index: Int, timestamp: Long, bitmap: Bitmap, uri: Uri?) {
            // Handle decoded frames
            // Do not recycle the bitmap image as it is reused internally
        }
    }
)

// Configure decoder
webPDecoder.configure(
    config = DecoderConfig(
        name_prefix = "IMAGE_",
        repeat_character = '0',
        repeat_character_count = 6,
        compressFormat = Bitmap.CompressFormat.PNG,
        compress_quality = 100
    )
)

// Decode a single frame
val decodeResult = webPDecoder.decodeNextFrame()

// Decode frames from a WebP file
webPDecoder.decodeFrames(dstUri)

// Decode only the image information from a WebP file
val info = webPDecoder.decodeInfo(srcUri)

// Release resources
webPDecoder.release()
```

## Appreciate my work!

If you find this library useful, please consider buying me a coffee.

<a href="https://www.buymeacoffee.com/udarawanasinghe" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>
