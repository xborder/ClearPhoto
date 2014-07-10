#ifndef HUE_COUNT_H_
#define HUE_COUNT_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct HueCounter {
	static int getHueCount(Mat& data);	
};

#endif 
