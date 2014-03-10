#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

#include <android/log.h>

#define LOG_TAG "AlgorithmExperiments"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

extern "C" {
JNIEXPORT void JNICALL Java_com_example_algorithmexperiments_ImageProcessJNIWrapper_canny(JNIEnv*, jobject, jlong addrGray, jlong addrRgba, jint threshold1, jint threshold2);
JNIEXPORT void JNICALL Java_com_example_algorithmexperiments_ImageProcessJNIWrapper_kmeans(JNIEnv*, jobject, jlong addrRgba);

Mat dst, detected_edges;
JNIEXPORT void JNICALL Java_com_example_algorithmexperiments_ImageProcessJNIWrapper_kmeans(JNIEnv*, jobject, jlong addrRgba)
{
	int width = (*(Mat*)addrRgba).cols;
	int height = (*(Mat*)addrRgba).rows;

	Mat src = Mat(width, height, CV_8UC4);

	blur(*(Mat*)addrRgba, src, Size(15,15));

	Mat p = Mat::zeros(src.cols*src.rows, 5, CV_32F);
	Mat bestLabels, centers, clustered;
	vector<Mat> bgr;
	cv::split(src, bgr);
	// i think there is a better way to split pixel bgr color
	for(int i=0; i<src.cols*src.rows; i++) {
		p.at<float>(i,0) = (i/src.cols) / src.rows;
		p.at<float>(i,1) = (i%src.cols) / src.cols;
		p.at<float>(i,2) = bgr[0].data[i] / 255.0;
		p.at<float>(i,3) = bgr[1].data[i] / 255.0;
		p.at<float>(i,4) = bgr[2].data[i] / 255.0;
	}

	int K = 3;
	cv::kmeans(p, K, bestLabels,TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 5, 0.8), 2, KMEANS_PP_CENTERS, centers);

	int colors[K];
	for(int i=0; i<K; i++) {
		colors[i] = 255/(i+1);
	}
	// i think there is a better way to do this mayebe some Mat::reshape?
	clustered = Mat(src.rows, src.cols, CV_32F);
	for(int i=0; i<src.cols*src.rows; i++) {
		clustered.at<float>(i/src.cols, i%src.cols) = (float)(colors[bestLabels.at<int>(0,i)]);
	}

	clustered.convertTo(clustered, CV_8U);

	clustered.copyTo(*(Mat*)addrRgba);
}

JNIEXPORT void JNICALL Java_com_example_algorithmexperiments_ImageProcessJNIWrapper_canny(JNIEnv*, jobject, jlong addrGray, jlong addrRgba, jint threshold1, jint threshold2)
{
	int edgeThresh = 1;
	int lowThreshold;
	int const max_lowThreshold = 100;
	int ratio = 3;
	int kernel_size = 3;

	/// Reduce noise with a kernel 3x3
	blur(*(Mat*)addrGray, detected_edges, Size(3,3));

	/// Canny detector
	Canny(detected_edges, detected_edges, threshold1, threshold2, kernel_size);

	//	  dst.create((*(Mat*)addrGray).size(), (*(Mat*)addrGray).type() );

	/// Using Canny's output as a mask, we display our result
	//	  dst = (*(Mat*)addrRgba).clone();

	detected_edges.copyTo(*(Mat*)addrRgba);
}
}
