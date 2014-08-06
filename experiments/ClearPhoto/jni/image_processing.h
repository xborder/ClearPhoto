/*
 * image_processing.h
 *
 *  Created on: Mar 27, 2014
 *      Author: helder
 */

#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cfloat>
#include <cmath>

#include "stdafx.h"
#include "ImageBalance/ImageBalance.h"
#include "HorizonDetection.h"
#include "MainLinesDetection.h"
#include "ObjectSegmentation.h"
#include "BackgroundSimplicity.h"
#include "HueCount.h"
#include "ColorTemplate/ColorTemplate.h"

#include <jni.h>
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
(JNIEnv* jenv, jclass, jlong data_, jintArray ret, jintArray ranges, jint width, jint height);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHueConcentration
(JNIEnv* jenv, jclass, jlong data_, jintArray ret, jintArray ranges, jint width, jint height);

JNIEXPORT jint JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetAvgSaturation
(JNIEnv* jenv, jclass, jlong data_, jint width, jint height);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_YUVtoBRGA
(JNIEnv* env, jclass, jint width, jint height, jbyteArray yuv, jintArray bgra);

void generateHistogram(const Mat* data, Mat* output);

// ############### HORIZON DETECTION CALLS ####################

JNIEXPORT jlong JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeInitDetectHorizon
(JNIEnv* jenv, jclass);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeStopDetectHorizon
(JNIEnv* jenv, jclass, jlong thiz);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectColorEdgeHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectColorHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectEdgeHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates);

// ############### HORIZON DETECTION CALLS ####################

// ############### MAJOR LINES CALLS ####################

JNIEXPORT jlong JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeInitMainLinesDetection
(JNIEnv* jenv, jclass);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeStopMainLinesDetection
(JNIEnv* jenv, jclass, jlong thiz);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetMainLines
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jint threshold, jobject list);

// ############### MAJOR LINES CALLS ####################

// ############### OBJECT SEGMENTATION CALLS ####################

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetSegmetationMask
(JNIEnv* jenv, jclass, jlong data_, jlong ret_);

// ############### OBJECT SEGMENTATION CALLS ####################

// ############### BACKGROUND SIMPLICITY CALLS ####################

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeTestBgSimplicity
(JNIEnv* jenv, jclass, jlong data_, jfloatArray ret);

// ############### BACKGROUND SIMPLICITY CALLS ####################

// ############### HUE COUNT CALLS ####################

JNIEXPORT jint JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHueCount
(JNIEnv* jenv, jclass, jlong data_);

// ############### HUE COUNT CALLS ####################

// ############### IMAGE BALANCE CALLS ####################

JNIEXPORT jint JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetImageBalance
(JNIEnv* jenv, jclass, jlong data_, jlong output_);

// ############### IMAGE BALANCE CALLS ####################

// ############### COLOR TEMPLATE CALLS ####################

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetColorTemplate
(JNIEnv* jenv, jclass, jlong data_, jobject list);

// ############### COLOR TEMPLATE CALLS ####################

}

#endif /* IMAGE_PROCESSING_H_ */
