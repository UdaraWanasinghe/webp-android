@file:Suppress("UnstableApiUsage")

import com.aureusapps.gradle.PublishLibraryConstants.GROUP_ID

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
    id("com.aureusapps.gradle.update-version")
    id("com.aureusapps.gradle.publish-library")
}

class Props(project: Project) {
    val groupId = project.findProperty(GROUP_ID).toString()
    val libwebpPath = project.findProperty("LIBWEBP_PATH").toString()
    val versionName = "1.0.6"
}

val props = Props(project)

android {
    compileSdk = 33
    namespace = "${props.groupId}.webpandroid"
    defaultConfig {
        minSdk = 21
        multiDexEnabled = true
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags("-std=c++17")
                arguments("-DLIBWEBP_PATH=${props.libwebpPath}")
            }
        }
    }
    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.27.4"
        }
    }
    publishing {
        singleVariant("release") {
            withSourcesJar()
            withJavadocJar()
        }
    }
}

publishLibrary {
    groupId.set(props.groupId)
    artifactId.set("webp-android")
    versionName.set(props.versionName)
    libName.set("WebPAndroid")
    libDescription.set("libwebp JNI bindings for Android.")
    libUrl.set("https://github.com/UdaraWanasinghe/webp-android")
    licenseName.set("MIT License")
    licenseUrl.set("https://github.com/UdaraWanasinghe/webp-android/blob/main/LICENSE")
    devId.set("UdaraWanasinghe")
    devName.set("Udara Wanasinghe")
    devEmail.set("udara.developer@gmail.com")
    scmConnection.set("scm:git:https://github.com/UdaraWanasinghe/webp-android.git")
    scmDevConnection.set("scm:git:ssh://git@github.com/UdaraWanasinghe/webp-android.git")
}

dependencies {
    implementation(libs.kotlin.stdlib)
    implementation(libs.annotation)
    implementation(libs.okhttp)
    implementation(libs.documentfile)
    implementation(libs.aureusapps.extensions)
    implementation(libs.relinker)

    testImplementation(libs.junit)

    androidTestImplementation(libs.junit.ext)
    androidTestImplementation(libs.espresso.core)
    androidTestImplementation(libs.core.ktx)
    androidTestImplementation(libs.aureusapps.extensions)
}