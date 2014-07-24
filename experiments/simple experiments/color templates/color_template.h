#ifndef COLOR_TEMPLATE_H_
#define COLOR_TEMPLATE_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <cvaux.h>
#include <highgui.h>

using namespace std;
using namespace cv;

struct ColorTemplate {
	static void getTemplate(Mat& data);	
};

#endif 
