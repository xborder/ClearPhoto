#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {

	Mat img = imread( argv[1], CV_LOAD_IMAGE_COLOR );
	if( img.data == NULL ) {
    cout << "Cannot load file " << argv[1] << endl;
    return 0;
  }
	Mat	hsv = Mat(img.size(), CV_8UC3);

	cvtColor(img, hsv, CV_BGR2HSV);
	vector<Mat> hsv_planes;
	split(hsv, hsv_planes);

	Mat sat = hsv_planes[1];

	int sum = 0;
	for(int i = 0; i < sat.rows * sat.cols; i++) {
		sum += (unsigned char)sat.data[i];
	}

	printf("%f\n",(sum/(double)(img.rows*img.cols)));
	//LOGE("width: %d, height: %d, sum: %d, avg:%d", width, height, sum, sum/(width*height));
	return sum/(img.rows*img.cols);
}
