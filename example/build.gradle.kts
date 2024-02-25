@file:Suppress("UnstableApiUsage")

import com.aureusapps.gradle.PublishLibraryConstants.GROUP_ID
import com.aureusapps.gradle.PublishLibraryConstants.VERSION_CODE
import com.aureusapps.gradle.PublishLibraryConstants.VERSION_NAME

plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
    id("com.aureusapps.gradle.update-version")
}

class Props(project: Project) {
    val groupId = project.findProperty(GROUP_ID).toString()
    val versionCode = project.findProperty(VERSION_CODE).toString().toInt()
    val versionName = project.findProperty(VERSION_NAME).toString()
}

val props = Props(project)

android {
    namespace = "${props.groupId}.webpandroid.example"
    compileSdk = 34
    defaultConfig {
        minSdk = 21
        versionCode = props.versionCode
        versionName = props.versionName
        multiDexEnabled = true
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }
    buildTypes {
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("debug")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
}

dependencies {
    implementation(libs.kotlin.stdlib)
    implementation(libs.core.ktx)
    implementation(libs.appcompat)
    implementation(libs.constraintlayout)
    implementation(libs.material)
    implementation(libs.fresco)
    implementation(libs.fresco.animated.webp)
    implementation(libs.fresco.webpsupport)
    implementation(libs.lifecycle.runtime.ktx)
    implementation(libs.aureusapps.extensions)
    implementation(libs.aureusapps.styles)
    implementation(libs.lifecycle.viewmodel.ktx)
    implementation(libs.documentfile)
    implementation(project(":webp-android"))

    testImplementation(libs.junit)

    androidTestImplementation(libs.junit.ext)
    androidTestImplementation(libs.espresso.core)
}