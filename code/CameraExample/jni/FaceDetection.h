#include <jni.h>

#ifndef _Included_helder_tese_cameraexample
#define _Included_helder_tese_cameraexample
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeCreateObject
  (JNIEnv *, jclass, jstring);

//JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_initObject
//  (JNIEnv *, jclass, jlong);

JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeDestroyObject
  (JNIEnv *, jclass, jlong);

//JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeStart
//  (JNIEnv *, jclass, jlong);
//
//JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeStop
//  (JNIEnv *, jclass, jlong);

JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeDetect
  (JNIEnv *, jclass, jlong, jint, jint, jbyteArray);

int* YUVtoRBG(JNIEnv *, jbyteArray, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
