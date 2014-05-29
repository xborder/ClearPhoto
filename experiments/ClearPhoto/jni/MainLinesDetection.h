/*
 * image_processing.h
 *
 *  Created on: Mar 27, 2014
 *      Author: helder
 */

#ifndef MAIN_LINES_DETECTION_H_
#define MAIN_LINES_DETECTION_H_

#include <opencv2/opencv.hpp>
#include <cmath>

#define DEG2RAD 0.017453293f
#define MAX_LINES 15

using namespace std;
using namespace cv;

class MainLinesDetection {
public:
	MainLinesDetection();
	~MainLinesDetection ();
	vector< pair< pair<int, int>, pair<int, int> > > getLines(Mat data, int threshold);
private:
	vector< pair< pair<int, int>, pair<int, int> > > processLines(Mat data, int threshold);
};
#endif
