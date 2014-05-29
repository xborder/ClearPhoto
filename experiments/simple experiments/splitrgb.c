#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]){
	VideoCapture capture(argv[1]);
	Mat frame;
	while(true){
		if(!capture.read(frame)){
			capture.set(CV_CAP_PROP_POS_MSEC, 0.0);
			continue;
		
		}
		Mat output (frame.size(),frame.type());
		vector<Mat> rgb;
		//vector<Mat> hsv;
		//vector<Mat> ycrcb;
		//cvtColor(frame,output,CV_RGB2YCrCb);
		namedWindow("Orginal");
		imshow("Orginal", frame);
		// split(frame, rgb);
		split(frame, rgb);

		namedWindow("b");
		namedWindow("g");
		namedWindow("r");

		imshow("b", rgb[0]);
		imshow("g", rgb[1]);
		imshow("r", rgb[2]);

		if(waitKey(25)>=0)
			break;
	}
	waitKey(0);
}
