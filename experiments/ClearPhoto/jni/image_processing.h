/*
 * image_processing.h
 *
 *  Created on: Mar 27, 2014
 *      Author: helder
 */

#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_

#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cfloat>
#include <cmath>

#include <android/log.h>

#define LOG_TAG "ImageProcessing"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

extern "C" {
JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeNearestPoint
(JNIEnv * jenv, jclass, jfloatArray points, jfloatArray faces, jfloatArray ret);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHistograms
(JNIEnv * jenv, jclass, jlong data_, jlong rHist, jlong ghist, jlong bHist, jlong grayHist, jint previewW, jint previewH);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetRGBHistogram
(JNIEnv * jenv, jclass, jlong data_, jlong rHist, jlong ghist, jlong bHist, jint previewW, jint previewH);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetGrayHistogram
(JNIEnv * jenv, jclass, jlong data_, jlong grayHist, jint previewW, jint previewH);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHueConcentration
(JNIEnv* jenv, jobject, jlong data_, jintArray ret, jintArray ranges, jint width, jint height);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_YUVtoBRGA
(JNIEnv* env, jobject, jint width, jint height, jbyteArray yuv, jintArray bgra);

void generateHistogram(const Mat* data, Mat* output);
}
#endif /* IMAGE_PROCESSING_H_ */
