#ifndef COLOR_TEMPLATE_H_
#define COLOR_TEMPLATE_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct ColorTemplate {
	static vector<int> getTemplate(Mat& data);

private:

	static vector<int> test90DegreesCluster(map<int,int> pallet, vector<pair<int,int> > sumBin, int sum, int startIndex);
	static vector<int> orderHistogram(Mat& histogram);
	static Mat getHistogram(Mat image, int hSize, int rangeI);
	static int mod(int a, int b) {
	    int r = a % b;
	    return r < 0 ? r + b : r;
	}
};

#endif
