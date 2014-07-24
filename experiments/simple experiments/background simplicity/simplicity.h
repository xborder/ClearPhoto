#ifndef BG_SIMPLICITY_H_
#define BG_SIMPLICITY_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include <cvaux.h>
#include <highgui.h>

using namespace std;
using namespace cv;

struct BackgroundSimplicity {
public:
	static float method1(Mat& data);	
	static float method2(Mat& data);	
	static float method3(Mat& data);	
private:
	static int getWidthOfMass(vector<float> vec, float total, int maxIdx);
};

#endif 