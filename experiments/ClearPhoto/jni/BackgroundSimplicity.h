#ifndef BG_SIMPLICITY_H_
#define BG_SIMPLICITY_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct BackgroundSimplicity {
public:
	static float method1(Mat& data); //Photo and Video Quality Evaluation: Focusing on the Subject
	static float method2(Mat& data); //Automatic Aesthetic Photo - Rating System
	static float method3(Mat& data); //The Design of High-Level Features for Photo Quality Assessment
private:
	static int getWidthOfMass(vector<float> vec, float total, int maxIdx);
};

#endif
