/*
 * image_processing.h
 *
 *  Created on: Mar 27, 2014
 *      Author: helder
 */

#ifndef HORIZON_DETECTION_H_
#define HORIZON_DETECTION_H_

//#include <jni.h>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
//#include <vector>
//#include <cfloat>
//#include <cmath>


#define DEG2RAD 0.017453293f
#define SKY_THRESHOLD 60 // sky probability over 60%

#define N_HORIZON_RESULTS 3

using namespace std;
using namespace cv;

class HorizonDetection {
public:
	HorizonDetection();
	~HorizonDetection();
	void horizonEdgeColorDetection(Mat* data, Mat* canny, int* ret);
	void horizonEdgeDetection(Mat* data, Mat* canny, int* ret, bool intensify);
	void horizonColorDetection(Mat* data, Mat* canny, int* ret, bool intensify);
private:
	int canny_min_threshold, canny_max_threshold;
	int median_rho, median_theta;
	void getLines(Mat data, int* ret, bool intensify);
	double houghSpaceDeviation(vector<int> v, double ave);
	double gaussian2D(int rho, int theta, double rho_deviation, double theta_deviation);
};
#endif
