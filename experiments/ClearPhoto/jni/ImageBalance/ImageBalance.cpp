
//
//  main.cpp
//  FSD
//
//  Created by Saburo Okita on 02/05/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#include "ImageBalance.h"

int ImageBalance::getImageBalance(const Mat& image, Mat& output) {
	/* Determine the shape of Hough accumulationmatrix */
	float rho_divs   = hypotf( image.rows, image.cols ) + 1;
	float theta_divs = 180.0;

	FastSymmetryDetector detector( image.size(), Size(rho_divs, theta_divs), 1 );

	Rect region( 0, image.rows, theta_divs * 2.0, rho_divs * 0.5 );
	Mat edge;

	/* Find the edges */
	cvtColor( image, edge, CV_BGR2GRAY );
	Canny( edge, edge, canny_thresh_1, canny_thresh_2 );

	/* Vote for the accumulation matrix */
	detector.vote( edge, min_pair_dist, max_pair_dist );

	//get point that form symmetry line
	pair<Point,Point> symmetryLine = getSymmetryLine(detector);
	line(output, symmetryLine.first, symmetryLine.second, Scalar(0, 0, 255, 255));
	return calculateSideWeight(image, symmetryLine);
}

pair<Point,Point> ImageBalance::getSymmetryLine(FastSymmetryDetector detector) {

	double maxVal = detector.getMaxVal();

	int iteration = 0;
	vector<pair<Point, Point> > result;
	float min_threshold = maxVal*(0.05*(iteration+1));
	float max_threshold = maxVal - (maxVal*(0.25*iteration));

	while( iteration < no_of_peaks)  {
		min_threshold = max_threshold*(0.05*(iteration+1));
		max_threshold = max_threshold - (max_threshold*(0.25*iteration));
		if(max_threshold < maxVal*0.5) {
			break;
		}

		vector<pair<Point, Point> > res = detector.getResult(1, min_threshold, max_threshold);
		iteration ++;
		if(res.size() > 0) {
			pair<Point, Point> p = res.front();
			result.push_back(p);
		}
	}
	Point mean1, mean2;
	int i = 0;
	for( auto point_pair: result ) {
		Point p1 = point_pair.first;
		Point p2 = point_pair.second;
		if (i == 0) {
//			line(temp, p1, p2, Scalar(255, 0, 0), 3);
			i++;
		} else {
//			line(temp, p1, p2, Scalar(0, 0, 255), 3);
		}
		mean1.x += p1.x;
		mean1.y += p1.y;
		mean2.x += p2.x;
		mean2.y += p2.y;
	}
	mean1.x /= result.size();
	mean1.y /= result.size();
	mean2.x /= result.size();
	mean2.y /= result.size();
//	line(temp, mean1, mean2, Scalar(0, 255, 255), 3);
	return pair<Point,Point>(mean1, mean2);
}

int ImageBalance::calculateSideWeight(const Mat& image, pair<Point, Point> symmetryLine) {
	int leftCount = 0, rightCount = 0;
	Mat leftMask = Mat::zeros(image.rows, image.cols, CV_8UC1);
	Mat rightMask = Mat::zeros(image.rows, image.cols, CV_8UC1);
	getDivisoryMask(image, symmetryLine, leftMask, rightMask, leftCount, rightCount);

	Mat left = Mat::zeros(image.size(), CV_8UC3);
	Mat right = Mat::zeros(image.size(), CV_8UC3);
	image.copyTo(left, leftMask);
	image.copyTo(right, rightMask);

	Mat leftSegMask, rightSegMask;
	ImageBalanceSegmentation::getSegmentationMask(left, leftMask, leftSegMask);
	ImageBalanceSegmentation::getSegmentationMask(right, rightMask, rightSegMask);

	int nLeftPixels = countNonZero(leftSegMask);
	int nRightPixels = countNonZero(rightSegMask);
	float leftPerc = (nLeftPixels/(float)(nLeftPixels + nRightPixels) * 100);
	float rightPerc = (nRightPixels/(float)(nLeftPixels + nRightPixels) * 100);

	printf("Left Occupation: %f%% Right Occupation: %f%% diff: %f%% \n", leftPerc, rightPerc, abs(leftPerc-rightPerc));
	if(abs(leftPerc-rightPerc) >= 70) {
		return UNBALANCED;
	} else if (abs(leftPerc-rightPerc) <= 30) {
		return BALANCED;
	} else {
		return VISUAL_TENSION;
	}
}

void ImageBalance::getDivisoryMask(const Mat& image, pair<Point,Point> symmetryLine, Mat& leftMask, Mat& rightMask, int& leftCount, int& rightCount) {
	Point p1 = symmetryLine.first;
	Point p2 = symmetryLine.second;

	if((p2.x - p1.x) == 0) { // é recta vertical
		unsigned char *leftoutput= (unsigned char*)(leftMask.data);
		unsigned char *rightoutput= (unsigned char*)(rightMask.data);
		for(int x = 0; x < image.cols; x++){
			for(int y = 0; y < image.rows; y++){
				if(x < p1.x) { // isLeft
					leftCount++;
					leftoutput[image.cols * y + x] = 255;
				} else { //isRight
					rightCount++;
					rightoutput[image.cols * y + x] = 255;
				}
			}
		}
		return ;
	}

	double m = -1/(double)((p2.y - p1.y) / (p2.x - p1.x));
	if ( m == 0.0) { // é recta horizontal
		unsigned char *leftoutput= (unsigned char*)(leftMask.data);
		unsigned char *rightoutput= (unsigned char*)(rightMask.data);
		for(int x = 0; x < image.cols; x++){
			for(int y = 0; y < image.rows; y++){
				if(y < p1.y) { // isLeft
					leftCount++;
					leftoutput[image.cols * y + x] = 255;
				} else { //isRight
					rightCount++;
					rightoutput[image.cols * y + x] = 255;
				}
			}
		}
	} else {
		int y0 = p1.y;
		int x0 = p1.x;

		int y1 = p2.y;
		int x1 = p2.x;

		unsigned char *leftoutput= (unsigned char*)(leftMask.data);
		unsigned char *rightoutput= (unsigned char*)(rightMask.data);
		for(int x = 0; x < image.cols; x++){
			for(int y = 0; y < image.rows; y++){
				if((x0 - x1)*(y - y1) < (y0 - y1)*(x - x1)) { // isRight
					rightCount++;
					rightoutput[image.cols * y + x] = 255;
				} else { //isLeft
					leftCount++;
					leftoutput[image.cols * y + x] = 255;
				}
			}
		}
	}
}
