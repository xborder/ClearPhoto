#include "image_processing.h"

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
	}
	jenv->SetFloatArrayRegion(ret, 0, faces_size, tmp_array);
	jenv->ReleaseFloatArrayElements(points, points_, 0);
	jenv->ReleaseFloatArrayElements(faces, faces_, 0);
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHistograms
(JNIEnv * jenv, jclass, jlong data, jlong rHist, jlong ghist, jlong bHist, jlong grayHist, jint previewW, jint previewH)
{

}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetRGBHistogram
(JNIEnv * jenv, jclass, jlong data_, jlong rHist_, jlong gHist_, jlong bHist_, jint previewW, jint previewH)
{
	Mat* rHist = (Mat*) rHist_;
	Mat* gHist = (Mat*) gHist_;
	Mat* bHist = (Mat*) bHist_;
	Mat* data = (Mat*) data_;
	Mat rgbData(previewH, previewW, CV_8UC1);

	cvtColor(*data, rgbData, CV_YUV420p2RGB, 3);

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> rgb_planes;
	split(rgbData, rgb_planes);

	generateHistogram(&rgb_planes[0], rHist);
	generateHistogram(&rgb_planes[1], gHist);
	generateHistogram(&rgb_planes[2], bHist);
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetGrayHistogram
(JNIEnv * jenv, jclass, jlong data_, jlong grayHist, jint previewW, jint previewH)
{

	Mat* data = (Mat*) data_;
	Mat grayData(previewH, previewW, CV_8UC1);
	cvtColor(*data, grayData, CV_YUV420p2GRAY, 1);

	Mat* output = (Mat*) grayHist;

	generateHistogram(&grayData, output);
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_YUVtoBRGA
(JNIEnv* env, jobject, jint width, jint height, jbyteArray yuv, jintArray bgra)
{
	jbyte* _yuv = env->GetByteArrayElements(yuv, 0);
	jint* _bgra = env->GetIntArrayElements(bgra, 0);
	Mat myuv = Mat(height + height/2, width, CV_8UC1, (uchar *)_yuv);
	Mat mbgra = Mat(height, width, CV_8UC4, (uchar *)_bgra);
	// Convert the color format from the camera's
	// NV21 "YUV420sp" format to an Android BGRA color image.
	cvtColor(myuv, mbgra, CV_YUV420sp2BGRA);
	// OpenCV can now access/modify the BGRA image "mbgra" ...
	env->ReleaseIntArrayElements(bgra, _bgra, 0);
	env->ReleaseByteArrayElements(yuv, _yuv, 0);
}

void generateHistogram(const Mat* data, Mat* output) {
    int bins = 256;
    int hsize[] = {bins};

    //max and min value of the histogram
    double max_value = 0, min_value = 0;

    //value and normalized value
    float value;
    int normalized;

    //ranges - grayscale 0 to 256
    float xranges[] = {0, 255};
    const float* ranges[] = {xranges};

    MatND hist;
    int channels[] = {0};
    calcHist(data, 1, channels, Mat(), hist, 1, hsize, ranges, true, false);
    normalize(hist, *output, 0, output->rows, NORM_MINMAX, -1, Mat());
//    normalize(hist, *output, 0, 1, NORM_MINMAX, -1, Mat());
}