# webp-android

[![Publish](https://github.com/UdaraWanasinghe/webp-android/actions/workflows/publish.yml/badge.svg?branch=main)](https://github.com/UdaraWanasinghe/webp-android/actions/workflows/publish.yml) ![Maven Central](https://img.shields.io/maven-central/v/com.aureusapps.android/webp-android)

## What's Inside?

✔️ Encode a series of android bitmap images to an animated WebP image.

## TODO

✘ Extract images from an animated WebP image.

## Using

1. Import the library into your project.

   ```groovy
   // module level build.gradle
   dependencies {
       implementation "com.aureusapps.android:webp-android:1.0.1"
   }
   ```

2. Create an encoder instance.

   ```kotlin
   val encoder = WebPAnimEncoder(
       width = 512,
       height = 512,
       WebPAnimEncoderOptions(
           animParams = WebPMuxAnimParams(loopCount = 0)
       )
   )
   ```

   See full list of encoder
   options [here](webp-android/src/main/java/com/aureusapps/android/webpandroid/encoder/WebPAnimEncoderOptions.kt).

3. Configure the encoder.

   ```kotlin
   encoder.configure(
       WebPConfig(
           lossless = WebPConfig.COMPRESSION_LOSSLESS,
           quality = 100f
       )
   )
   ```

   See full list of encoder
   configurations [here](webp-android/src/main/java/com/aureusapps/android/webpandroid/encoder/WebPConfig.kt).

4. Add encoder progress listener.

   ```kotlin
   encoder.addProgressListener(object : WebPAnimEncoderListener {
       override fun onProgressUpdate(framePercent: Int, currentFrame: Int) {
           // update progress here
       }
   })
   ```

5. Add frames with their timestamp.

   ```kotlin
   encoder.addFrame(WebPFrame(bitmap1, 0))
   encoder.addFrame(WebPFrame(bitmap2, 1000))
   ```

6. Call assemble by giving the last timestamp and the save path.

   ```kotlin
   encoder.assemble(2000, path)
   ```

7. Release the encoder resources.

   ```kotlin
   encoder.release()
   ```

## Updating version

```shell
./gradlew updateVersion --code=xx --name=xx.xx.xx
```

## Appreciate my work!

If you find this library useful, please consider buying me a coffee.

<a href="https://www.buymeacoffee.com/udarawanasinghe" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>