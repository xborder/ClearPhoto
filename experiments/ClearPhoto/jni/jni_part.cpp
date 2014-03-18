#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cfloat>
#include <cmath>

#include <android/log.h>

#define LOG_TAG "ImageProcessing"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

extern "C" {
JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeNearestPoint (JNIEnv * jenv, jclass, jfloatArray points, jfloatArray faces, jfloatArray nearest);
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeNearestPoint
(JNIEnv * jenv, jclass, jfloatArray points, jfloatArray faces, jfloatArray nearest)
{
	jfloat* points_ = jenv->GetFloatArrayElements(points,0);
	jfloat* faces_  = jenv->GetFloatArrayElements(faces,0);

	float x0 = faces_[0], y0 = faces_[1];
	jsize points_size = jenv->GetArrayLength(points);
	jfloat nx = FLT_MAX, ny = FLT_MAX;
	float min_d = FLT_MAX, tmp_d = 0.0f;
    LOGD(">>>>> ");
	for(int i = 0; i < points_size; i+=2) {
		tmp_d = sqrt(pow(points_[i]-x0,2) + pow(points_[i+1]-y0,2));

	    LOGD(">>>>> %f %f %f", tmp_d, points_[i], points_[i+1]);
		if(tmp_d < min_d) {
			min_d = tmp_d;
			nx = points_[i];
			ny = points_[i+1];
		}
	}

    LOGD(">>>>> ");
	jenv->ReleaseFloatArrayElements(points, points_, 0);
	jenv->ReleaseFloatArrayElements(faces, faces_, 0);
	jenv->SetFloatArrayRegion(nearest, 0, 1, &nx);
	jenv->SetFloatArrayRegion(nearest, 1, 1, &ny);
}
