@file:Suppress("UnstableApiUsage")

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
    `maven-publish`
    signing
}

class Props(project: Project) {
    val groupId = project.findProperty("GROUP_ID") as String
    val libwebpPath = project.findProperty("LIBWEBP_PATH") as String
    val versionName = project.findProperty("VERSION_NAME") as String
    val signingKeyId = project.findProperty("SIGNING_KEY_ID") as String?
    val signingKey = project.findProperty("SIGNING_KEY") as String?
    val signingPassword = project.findProperty("SIGNING_PASSWORD") as String?
}

val props = Props(project)

project.group = props.groupId
project.version = props.versionName

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
                arguments("-DLIBWEBP_PATH=${props.libwebpPath}")
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

publishing {
    publications {
        create<MavenPublication>("releasePublication") {
            groupId = props.groupId
            artifactId = "webp-android"
            version = props.versionName
            from(project.components.findByName("release"))

            pom {
                name = "WebPAndroid"
                description = "libwebp JNI bindings for Android."
                url = "https://github.com/UdaraWanasinghe/webp-android"

                licenses {
                    license {
                        name = "MIT License"
                        url = "https://github.com/UdaraWanasinghe/webp-android/blob/main/LICENSE"
                    }
                }

                developers {
                    developer {
                        id = "UdaraWanasinghe"
                        name = "Udara Wanasinghe"
                        email = "udara.developer@gmail.com"
                    }
                }

                scm {
                    connection = "scm:git:https://github.com/UdaraWanasinghe/webp-android.git"
                    developerConnection = "scm:git:ssh://git@github.com/UdaraWanasinghe/webp-android.git"
                    url = "https://github.com/UdaraWanasinghe/webp-android"
                }
            }
        }
    }
}

signing {
    useInMemoryPgpKeys(props.signingKeyId, props.signingKey, props.signingPassword)
    sign(publishing.publications)
}

dependencies {
    implementation(libs.kotlin.stdlib)
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.annotation)
    implementation(libs.okhttp)
    implementation(libs.relinker)

    androidTestImplementation(libs.androidx.core.ktx)
    androidTestImplementation(libs.androidx.test.ext.junit)
    androidTestImplementation(libs.androidx.test.espresso.core)
}
