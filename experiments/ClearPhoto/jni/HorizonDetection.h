/*
 * image_processing.h
 *
 *  Created on: Mar 27, 2014
 *      Author: helder
 */

#ifndef HORIZON_DETECTION_H_
#define HORIZON_DETECTION_H_

#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cfloat>
#include <cmath>

#include <android/log.h>

#define DEG2RAD 0.017453293f
#define SKY_THRESHOLD 60 // sky probability over 60%

using namespace std;
using namespace cv;

class HorizonDetection {
public:
	HorizonDetection();
	virtual ~HorizonDetection();
	void horizonEdgeDetection(Mat* data, int* ret);
	void horizonColorDetection(Mat* data, int* output, double (*intensifyFunction)(int,int,double,double));
	void horizonEdgeColorDetection();
private:
	int width, height;

	void getLines();
}
#endif
