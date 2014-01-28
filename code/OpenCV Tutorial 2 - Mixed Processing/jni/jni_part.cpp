#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "DetectionBasedTracker_jni.h"

#include <android/log.h>

#define LOG_TAG "Mixed_SampleMixed_SampleMixed_Sample"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

extern "C" {
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_CalcExposure(JNIEnv*, jobject, jlong addrHsv, jlong addrRgba);
void calc(Mat v, Mat h, Mat s, double* exposure, double* saturation, double* hue);

JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
    Mat& mGr  = *(Mat*)addrGray;
    Mat& mRgb = *(Mat*)addrRgba;
    vector<KeyPoint> v;

    FastFeatureDetector detector(50);
    detector.detect(mGr, v);
    for( unsigned int i = 0; i < v.size(); i++ )
    {
        const KeyPoint& kp = v[i];
        circle(mRgb, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
    }
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
