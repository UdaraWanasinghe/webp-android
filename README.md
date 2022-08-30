[![android library](https://img.shields.io/badge/version-v1.0.0-orange)](https://github.com/UdaraWanasinghe/webp-android)



## What's Included?

✅ Create an animated WebP image from a series of android bitmap images.



## TODO

⬜ Extract images from an animated WebP image.

⬜ Add unit test cases.



## Building

1. Clone libwebp library.
```shell
git clone https://chromium.googlesource.com/webm/libwebp
```
2. Clone this repository.

```shell
git clone https://github.com/UdaraWanasinghe/webp-android.git
```

3. Run `gradle` task `publishToMavenLocal` in webp-android project.
```shell
cd webp-android
sh gradlew publishToMavenLocal
```

Note that libwebp project should be in the same directory as webp-android project.