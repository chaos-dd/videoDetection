/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class videoDetectionJNI_VideoDetecion */

#ifndef _Included_videoDetectionJNI_VideoDetecion
#define _Included_videoDetectionJNI_VideoDetecion
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    initModule
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_videoDetectionJNI_VideoDetecion_initModule
  (JNIEnv *, jclass, jstring);

/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    getFrame
 * Signature: (Ljava/lang/String;ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_videoDetectionJNI_VideoDetecion_getFrame
  (JNIEnv *, jclass, jstring, jint, jstring);

/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    processSingleVideo
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_videoDetectionJNI_VideoDetecion_processSingleVideo
  (JNIEnv *, jclass, jstring);

/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    detectSingleVideo
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_videoDetectionJNI_VideoDetecion_detectSingleVideo__Ljava_lang_String_2
  (JNIEnv *, jclass, jstring);

/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    detectSingleVideo
 * Signature: (Ljava/lang/String;[Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_videoDetectionJNI_VideoDetecion_detectSingleVideo__Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *, jclass, jstring, jobjectArray);

/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    detectMultipleVideos
 * Signature: ([Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_videoDetectionJNI_VideoDetecion_detectMultipleVideos___3Ljava_lang_String_2
  (JNIEnv *, jclass, jobjectArray);

/*
 * Class:     videoDetectionJNI_VideoDetecion
 * Method:    detectMultipleVideos
 * Signature: ([Ljava/lang/String;[Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_videoDetectionJNI_VideoDetecion_detectMultipleVideos___3Ljava_lang_String_2_3Ljava_lang_String_2
  (JNIEnv *, jclass, jobjectArray, jobjectArray);

#ifdef __cplusplus
}
#endif
#endif
