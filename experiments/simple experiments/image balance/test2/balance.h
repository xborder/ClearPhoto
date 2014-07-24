#ifndef IMAGE_BALANCE_H_
#define IMAGE_BALANCE_H_

#include <iostream>
#include <stdio.h>
#include <queue>
#include <stdlib.h>

#include <cvaux.h>
#include <highgui.h>

using namespace std;
using namespace cv;

class HoughPoint {
public:
	int theta;
	int rho;
	int val;
	HoughPoint(int theta, int rho, int val)
		: theta(theta), rho(rho), val(val)
	{}
};

struct HoughPointCompare {
	bool operator()(const HoughPoint &p1, const HoughPoint &p2) const {
	return p1.val < p2.val;
	}
};

struct ImageBalance {
	static void getBalance(const Mat& image);
};

#endif 
