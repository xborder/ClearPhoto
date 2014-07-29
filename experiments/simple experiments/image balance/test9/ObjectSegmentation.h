#ifndef OBJECT_SEGMENTATION_H_
#define OBJECT_SEGMENTATION_H_

#include "stdafx.h"


#define BINARY_THRESHOLD 255 * 0.8
#define MIN_FGD_THRESHOLD 		200
#define MIN_PR_BGD_THRESHOLD 	20

struct ObjectSegmentation {

	static void getSegmentationMask(const Mat& image, Mat& mask);
	static Mat GetHC(const Mat &img3f);

	// Region Contrast
	static Mat GetRC(const Mat &img3f);
	static Mat GetRC(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma);

private:
	static void getBinaryImage(const Mat& saliency, Mat& binary, Point& center, Rect& rect);
	static void GetHC(const Mat &binColor3f, const Mat &weights1f, Mat &colorSaliency);
 	static void SmoothSaliency(const Mat &binColor3f, Mat &sal1d, float delta, const vector<vector<CostfIdx> > &similar);
 	static int Quantize(const Mat& img3f, Mat &idx1i, Mat &_color3f, Mat &_colorNum, double ratio = 0.95);

	struct Region{
		Region() { pixNum = 0;}
		int pixNum;  // Number of pixels
		vector<CostfIdx> freIdx;  // Frequency of each color and its index
		Point2d centroid;
	};
	static void BuildRegions(const Mat& regIdx1i, vector<Region> &regs, const Mat &colorIdx1i, int colorNum);
	static void RegionContrast(const vector<Region> &regs, const Mat &color3fv, Mat& regSal1d, double sigmaDist);
};

#endif
