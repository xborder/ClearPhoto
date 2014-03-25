#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

#include <android/log.h>

#define LOG_TAG "Mixed_SampleMixed_SampleMixed_Sample"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

extern "C" {
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_WaterShed(JNIEnv*, jobject, jlong addrRgba);
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_CalcExposure(JNIEnv*, jobject, jlong addrHsv, jlong addrRgba);
void calc(Mat v, Mat h, Mat s, double* exposure, double* saturation, double* hue);

class WatershedSegmenter{
private:
	cv::Mat markers;
public:
	void setMarkers(cv::Mat& markerImage)
	{
		markerImage.convertTo(markers, CV_32S);
	}

	cv::Mat process(cv::Mat &image)
	{
		cv::watershed(image, markers);
		markers.convertTo(markers,CV_8U);
		return markers;
	}
};

JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_WaterShed(JNIEnv*, jobject, jlong addrRgba)
{
	cv::Mat& image = *(Mat*)addrRgba;

	// Create binary image from source image
	cv::Mat bw;
	cv::cvtColor(image, bw, CV_BGR2GRAY);
	cv::threshold(bw, bw, 40, 255, CV_THRESH_BINARY);

	cv::Mat dist;
	cv::distanceTransform(bw, dist, CV_DIST_L2, 3);
	cv::normalize(dist, dist, 0, 1., cv::NORM_MINMAX);

	cv::threshold(dist, dist, .5, 1., CV_THRESH_BINARY);

	// Create the CV_8U version of the distance image
	// It is needed for cv::findContours()
	cv::Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);

	// Find total markers
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// Total objects
	int ncomp = contours.size();

	cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32SC1);
	for (int i = 0; i < ncomp; i++)
	    cv::drawContours(markers, contours, i, cv::Scalar::all(i+1), -1);

	cv::circle(markers, cv::Point(5,5), 3, CV_RGB(255,255,255), -1);

	cv::watershed(image, markers);

	// Generate random colors
	std::vector<cv::Vec3b> colors;
	for (int i = 0; i < ncomp; i++)
	{
	    int b = cv::theRNG().uniform(0, 255);
	    int g = cv::theRNG().uniform(0, 255);
	    int r = cv::theRNG().uniform(0, 255);

	    colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	// Create the result image
	cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);

	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
	    for (int j = 0; j < markers.cols; j++)
	    {
	        int index = markers.at<int>(i,j);
	        if (index > 0 && index <= ncomp)
	            dst.at<cv::Vec3b>(i,j) = colors[index-1];
	        else
	            dst.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
	    }
	}

	dst.copyTo(image);
}

JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_CalcExposure(JNIEnv*, jobject, jlong addrHsv, jlong addrRgba)
{

	Mat& hsv = *(Mat*)addrHsv;
	vector<Mat> hsv_planes;
	split( hsv, hsv_planes );
	cv::Mat h_image = hsv_planes[0];
	cv::Mat s_image = hsv_planes[1];
	cv::Mat v_image = hsv_planes[2];

	double exposure = 0.0;
	double saturation = 0.0;
	double hue = 0.0;

	calc(v_image, h_image, s_image, &exposure, &saturation, &hue);

	std::ostringstream s1, s2, s3;
	s1 << "Avg. Exposure: " << exposure;
	s2 << "Avg. Saturation:" << saturation;
	s3 << "Avg. Hue: " << hue;
	putText(*(Mat*)addrRgba, s1.str().c_str(), Point(50,50), CV_FONT_HERSHEY_DUPLEX, 1, Scalar::all(255));
	putText(*(Mat*)addrRgba, s2.str().c_str(), Point(50,75), CV_FONT_HERSHEY_DUPLEX, 1, Scalar::all(255));
	putText(*(Mat*)addrRgba, s3.str().c_str(), Point(50,100), CV_FONT_HERSHEY_DUPLEX, 1, Scalar::all(255));
}

void calc(Mat v, Mat h, Mat s, double* exposure, double* saturation, double* hue) {
	int size = *v.size;
	int sum_e = 0, sum_s = 0, sum_h = 0;

	for(int i = 0; i < v.rows; i++) {
		for(int j = 0; j < v.cols; j++) {
			sum_e += v.data[i*v.cols + j];
			sum_h += h.data[i*v.cols + j];
			sum_s += s.data[i*v.cols + j];
		}
	}

	*exposure = sum_e/size;
	*saturation = sum_s/size;
	*hue = sum_h/size;
}
}
