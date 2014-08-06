#ifndef IMAGE_BALANCE_H_
#define IMAGE_BALANCE_H_

#include <opencv2/opencv.hpp>

#include "FastSymmetryDetector.h"
#include "ImageBalanceSegmentation.h"

using namespace std;
using namespace cv;


struct ImageBalance {
	static const int UNBALANCED = 0, VISUAL_TENSION = 1, BALANCED = 2, NONE = -1;
	static int getImageBalance(const Mat& image, Mat& output);
private:
	static const int canny_thresh_1 = 182;
	static const int canny_thresh_2 = 350;
	static const int min_pair_dist  = 25;
	static const int max_pair_dist  = 500;
	static const int no_of_peaks    = 3;

	static pair<Point,Point> getSymmetryLine(FastSymmetryDetector detector);
	static int calculateSideWeight(const Mat& image, pair<Point, Point> symmetryLine);
	static void getDivisoryMask(const Mat& image, pair<Point,Point> symmetryLine, Mat& leftMask, Mat& rightMask, int& leftCount, int& rightCount);
};

#endif
