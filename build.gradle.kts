project.extra["GROUP_ID"] = "com.aureusapps.android"
project.extra["VERSION_CODE"] = 13
project.extra["VERSION_NAME"] = "1.1.2"
project.extra["LIBWEBP_PATH"] = "../../../../../libwebp"

plugins {
    alias(libs.plugins.com.android.library) apply false
    alias(libs.plugins.org.jetbrains.kotlin.android) apply false
    alias(libs.plugins.com.vanniktech.maven.publish) apply false
}