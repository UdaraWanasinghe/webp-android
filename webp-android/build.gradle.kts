@file:Suppress("UnstableApiUsage")

import com.aureusapps.gradle.PublishLibraryConstants.GROUP_ID
import com.aureusapps.gradle.PublishLibraryConstants.VERSION_NAME

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
    id("com.aureusapps.gradle.update-version")
    id("com.aureusapps.gradle.publish-library")
}

class Props(project: Project) {
    val groupId = project.findProperty(GROUP_ID) as String
    val libWEBPPath = project.findProperty("LIBWEBP_PATH") as String
    val versionName = project.findProperty(VERSION_NAME) as String
}

val props = Props(project)

android {
    compileSdk = 35
    namespace = "${props.groupId}.webpandroid"
    defaultConfig {
        minSdk = 21
        multiDexEnabled = true
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags("-std=c++17", "-fvisibility=hidden")
                arguments("-DLIBWEBP_PATH=${props.libWEBPPath}")
                targets("webpcodec_jni")
            }
        }
        consumerProguardFiles("consumer-rules.pro")
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
            version = "3.26.4+"
        }
    }
    buildFeatures {
        buildConfig = true
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
    implementation(libs.androidx.annotation)
    implementation(libs.okhttp)
    implementation(libs.androidx.documentfile)
    implementation(libs.aureusapps.extensions)
    implementation(libs.relinker)

    androidTestImplementation(libs.androidx.test.ext.junit)
    androidTestImplementation(libs.androidx.test.espresso.core)
    androidTestImplementation(libs.androidx.core.ktx)
    androidTestImplementation(libs.aureusapps.extensions)
}
