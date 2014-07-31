#ifndef IMAGEBALANCE_SEGMENTATION_H_
#define IMAGEBALANCE_SEGMENTATION_H_

#include "stdafx.h"

#define MIN_PR_FGD_THRESHOLD	200
#define MIN_PR_BGD_THRESHOLD 	20

struct ImageBalanceSegmentation {
	static void getSegmentationMask(const Mat& image, const Mat& mask, Mat& outputMask);
	static Mat GetHC(const Mat &img3f, const Mat& mask);

private:
	static void getBinaryImage(const Mat& saliency, Mat& binary, Point& center, Rect& rect);
	static void GetHC(const Mat &binColor3f, const Mat &weights1f, Mat &colorSaliency);
 	static void SmoothSaliency(const Mat &binColor3f, Mat &sal1d, float delta, const vector<vector<CostfIdx> > &similar);
 	static int Quantize(const Mat& img3f, const Mat& mask, Mat &idx1i, Mat &_color3f, Mat &_colorNum, double ratio = 0.95);
};

#endif
