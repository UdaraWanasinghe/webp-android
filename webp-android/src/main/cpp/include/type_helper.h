//
// Created by udara on 6/4/23.
//

#pragma once

#include <jni.h>

namespace type {
    /**
     * @brief Retrieves the integer value from a Java object.
     *
     * This function extracts the integer value from a Java object of type Integer and returns it as an int.
     *
     * @param env A pointer to the JNI environment.
     * @param object The Java object from which to retrieve the integer value.
     *
     * @return The integer value extracted from the Java object.
     *
     * @throws std::runtime_error if the given object is not of type Integer.
     */
    int getIntegerValue(JNIEnv *env, jobject object);

    /**
     * @brief Retrieves the boolean value from a Java object.
     *
     * This function extracts the boolean value from a Java object of type Boolean and returns it as a bool.
     *
     * @param env A pointer to the JNI environment.
     * @param object The Java object from which to retrieve the boolean value.
     *
     * @return The boolean value extracted from the Java object.
     *
     * @throws std::runtime_error if the given object is not of type Boolean.
     */
    bool getBooleanValue(JNIEnv *env, jobject object);

    /**
     * @brief Retrieves the float value from a Java object.
     *
     * This function extracts the float value from a Java object of type Float and returns it as a float.
     *
     * @param env A pointer to the JNI environment.
     * @param object The Java object from which to retrieve the float value.
     *
     * @return The float value extracted from the Java object.
     *
     * @throws std::runtime_error if the given object is not of type Float.
     */
    float getFloatValue(JNIEnv *env, jobject object);

    /**
     * @brief Retrieves the value of an object field.
     *
     * This function retrieves the value of a specific field from a Java object and returns it as a jobject.
     *
     * @param env A pointer to the JNI environment.
     * @param object The Java object from which to retrieve the field value.
     * @param name The name of the field to retrieve.
     * @param sig The signature of the field.
     *
     * @return The value of the specified object field.
     */
    jobject getObjectField(
            JNIEnv *env,
            jobject object,
            const char *name,
            const char *sig
    );

    /**
     * @brief Checks if a Java object is null.
     *
     * This function checks whether a Java object is null and returns a boolean indicating the result.
     *
     * @param env A pointer to the JNI environment.
     * @param obj The Java object to check for null.
     *
     * @return true if the Java object is null, false otherwise.
     */
    bool isObjectNull(JNIEnv *env, jobject obj);
}