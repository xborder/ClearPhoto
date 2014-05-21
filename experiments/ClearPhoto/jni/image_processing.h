/*
 * image_processing.h
 *
 *  Created on: Mar 27, 2014
 *      Author: helder
 */

#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_

#include <jni.h>
#include <android/log.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cfloat>
#include <cmath>

#include "HorizonDetection.h"


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


//void horizonEdgeDetection(Mat* data, int* ret,  double (*)(int,int,double,double));
//void horizonColorDetection(Mat* data, int* output, int width, int height, double (*intensifyFunction)(int,int,double,double));
//void applyHough(Mat binary_image, int* output, double (*)(int,int,double,double));

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_YUVtoBRGA
(JNIEnv* env, jclass, jint width, jint height, jbyteArray yuv, jintArray bgra);

void generateHistogram(const Mat* data, Mat* output);

//###########################################################################

JNIEXPORT jlong JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeInitDetectHorizon
(JNIEnv* jenv, jclass);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeStopDetectHorizon
(JNIEnv* jenv, jclass);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectColorEdgeHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectColorHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates);

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectEdgeHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates);
}



#endif /* IMAGE_PROCESSING_H_ */
