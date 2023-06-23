# webp-android

[![Publish](https://github.com/UdaraWanasinghe/webp-android/actions/workflows/publish.yml/badge.svg?branch=main)](https://github.com/UdaraWanasinghe/webp-android/actions/workflows/publish.yml)
[![Maven Central](https://img.shields.io/maven-central/v/com.aureusapps.android/webp-android)](https://central.sonatype.com/artifact/com.aureusapps.android/webp-android/1.0.1)
[![javadoc](https://javadoc.io/badge2/com.aureusapps.android/webp-android/javadoc.svg)](https://javadoc.io/doc/com.aureusapps.android/webp-android)

## What's Inside

* Encode a series of android bitmap images to a static or an animated WebP image.
* Extract bitmap images from an animated WebP image.

## Using

### Import the library into your project.

```groovy
// module level build.gradle
dependencies {
    implementation "com.aureusapps.android:webp-android:1.0.1"
}
```

### Encoding a Bitmap with WebPEncoder

```kotlin
// Create encoder instance
val webPEncoder = WebPEncoder(width, height)

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
webPEncoder.encode(context, srcBitmap, dstUri)

// Release resources
webPEncoder.release()
```

### Encoding a list of Bitmap with WebPAnimEncoder

```kotlin
val webPAnimEncoder = WebPAnimEncoder(
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
webPAnimEncoder.addFrame(context, timestamp, srcUri)

// Assemble the animation
webPAnimEncoder.assemble(context, timestamp, dstUri)

// Release resources
webPAnimEncoder.release()
```

### Decoding WebP Image with WebPDecoder

```kotlin
// Create decoder instance
val webPDecoder = WebPDecoder()

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

// Decode frames from a WebP file
webPDecoder.decodeFrames(context, srcUri, dstUri)

// Decode only the image information from a WebP file
val info = webPDecoder.decodeInfo(context, srcUri)

// Release resources
webPDecoder.release()
```

## Updating the version

```shell
./gradlew updateVersion --code=xx --name=xx.xx.xx
```

## Appreciate my work!

If you find this library useful, please consider buying me a coffee.

<a href="https://www.buymeacoffee.com/udarawanasinghe" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>