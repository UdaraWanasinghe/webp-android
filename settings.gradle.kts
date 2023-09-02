@file:Suppress("UnstableApiUsage")

pluginManagement {
    repositories {
        gradlePluginPortal()
        google()
        mavenCentral()
        mavenLocal()
    }
}
buildscript {
    repositories {
        gradlePluginPortal()
        mavenLocal()
    }
    dependencies {
        classpath("com.aureusapps.gradle:plugin-utils:1.0.0")
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    versionCatalogs {
        create("libs") {
            from("com.aureusapps:version-catalog:1.0.0")
        }
    }
    repositories {
        google()
        mavenCentral()
        mavenLocal()
    }
}
rootProject.name = "webp-android"
include("example", "webp-android", "webp-android-simple")