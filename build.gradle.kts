import java.io.FileInputStream
import java.util.Properties

fun loadLocalProperties(project: Project) {
    val rootProject = project.rootProject
    val file = rootProject.file("local.properties")
    if (file.exists()) {
        FileInputStream(file).use { inputStream ->
            val properties = Properties()
            properties.load(inputStream)
            val extraProperties = rootProject.extensions.extraProperties
            properties.forEach { key, value ->
                extraProperties[key.toString()] = value.toString()
            }
        }
    }
}

loadLocalProperties(project)
project.extra["GROUP_ID"] = "com.aureusapps.android"
project.extra["VERSION_CODE"] = 12
project.extra["VERSION_NAME"] = "1.1.1"
project.extra["LIBWEBP_PATH"] = "../../../../../libwebp"

plugins {
    alias(libs.plugins.com.android.library) apply false
    alias(libs.plugins.org.jetbrains.kotlin.android) apply false
    alias(libs.plugins.io.github.gradle.nexus.publish.plugin) apply true
}

nexusPublishing {
    repositories {
        sonatype {
            username = project.findProperty("OSSRH_USERNAME") as String?
            password = project.findProperty("OSSRH_PASSWORD") as String?
            stagingProfileId = project.findProperty("STAGING_PROFILE_ID") as String?
        }
    }
}