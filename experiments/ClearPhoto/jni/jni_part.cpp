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
JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeNearestPoint (JNIEnv * jenv, jclass, jfloatArray points, jfloatArray faces, jfloatArray ret);
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeNearestPoint
(JNIEnv * jenv, jclass, jfloatArray points, jfloatArray faces, jfloatArray ret)
{

	jsize points_size = jenv->GetArrayLength(points), faces_size = jenv->GetArrayLength(faces);

	jfloat* points_ = jenv->GetFloatArrayElements(points,0);
	jfloat* faces_  = jenv->GetFloatArrayElements(faces,0);

	jfloat tmp_array[faces_size];
	float x0, y0;
	float nx, ny;
	float min_d, tmp_d = 0.0f;
	for(int j = 0; j < faces_size; j+=2) {

		LOGD(">>>>>>>>>>>>>>>< %d %d", points_size, faces_size);
		x0 = faces_[j]; y0 = faces_[j+1];
		min_d = FLT_MAX; nx = FLT_MAX; ny = FLT_MAX;
		for(int i = 0; i < points_size; i+=2) {
			tmp_d = sqrt(pow(points_[i]-x0,2) + pow(points_[i+1]-y0,2));

			if(tmp_d < min_d) {
				min_d = tmp_d;
				nx = points_[i];
				ny = points_[i+1];
			}
		}
		tmp_array[j] = nx;
		tmp_array[j+1] = ny;

		LOGD(">>>>> %f %f", nx,ny);

//		jenv->SetFloatArrayRegion(nearest, ret_index++, 1, &nx);
//		jenv->SetFloatArrayRegion(nearest, ret_index++, 1, &ny);
	}
	jenv->SetFloatArrayRegion(ret, 0, faces_size, tmp_array);
	jenv->ReleaseFloatArrayElements(points, points_, 0);
	jenv->ReleaseFloatArrayElements(faces, faces_, 0);
}
