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
(JNIEnv * jenv, jclass class_, jlong data, jlong rHist, jlong ghist, jlong bHist, jlong grayHist, jint previewW, jint previewH)
{
	Java_tese_helder_clearphoto_ImageProcessing_nativeGetRGBHistogram(jenv, class_, data, rHist, ghist, bHist, previewW, previewH);
	Java_tese_helder_clearphoto_ImageProcessing_nativeGetGrayHistogram(jenv, class_, data, grayHist, previewW, previewH);
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

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHueConcentration
(JNIEnv* jenv, jclass, jlong data_, jintArray ret, jintArray ranges, jint width, jint height)
{
	jsize ranges_size = jenv->GetArrayLength(ranges);
	jsize ret_size = jenv->GetArrayLength(ret);
	jint* ranges_ = jenv->GetIntArrayElements(ranges,0);

	int* tmp_array = (int*)calloc(sizeof(int), ret_size);

	Mat* data = (Mat*) data_;
	Mat rgb = Mat(height, width, CV_8UC3);
	Mat	hsv = Mat(height, width, CV_8UC3);

	cvtColor(*data, rgb, CV_YUV420p2BGR);
	cvtColor(rgb, hsv, CV_BGR2HSV);

	vector<Mat> hsv_planes;
	split(hsv, hsv_planes);

	Mat hue = hsv_planes[0];
	int lower_range = 0, upper_range = 0;
	for(int i = 0; i < hue.rows; i++) {
		for(int j = 0; j < hue.cols; j++) {
			uchar val = hue.at<uchar>(i,j);
			for(int r = 0 ; r < ranges_size; r++) {
				lower_range = ranges_[r];
				upper_range = ranges_[(r + 1) % ranges_size];
				if(val >= lower_range && val < upper_range) {
					tmp_array[r]++;
					break;
				} else if( (r + 1) % ranges_size == 0 && (val >= lower_range || val < upper_range)){
					tmp_array[r]++;
				}
			}
		}
	}

	jenv->SetIntArrayRegion(ret, 0, ret_size, tmp_array);
	jenv->ReleaseIntArrayElements(ranges, ranges_, 0);
}

JNIEXPORT jint JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetAvgSaturation
(JNIEnv* jenv, jclass cls, jlong data_, jint width, jint height)
{
	Mat* data = (Mat*) data_;
	Mat rgba = Mat(height, width, CV_8UC4);
	Mat	hsv = Mat(height, width, CV_8UC3);

	cvtColor(*data, rgba, CV_YUV420p2BGRA);
	cvtColor(rgba, hsv, CV_BGR2HSV);
	vector<Mat> hsv_planes;
	split(hsv, hsv_planes);

	Mat sat = hsv_planes[1];
	int sum = 0;
	for(int i = 0; i < sat.rows * sat.cols; i++) {
		sum += (unsigned char)sat.data[i];
	}
	//LOGE("width: %d, height: %d, sum: %d, avg:%d", width, height, sum, sum/(width*height));
	return sum/(width*height);
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_YUVtoBRGA
(JNIEnv* env, jclass, jint width, jint height, jbyteArray yuv, jintArray bgra)
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
	normalize(hist, *output, 0, 255, NORM_MINMAX, -1, Mat());
	//    normalize(hist, *output, 0, 1, NORM_MINMAX, -1, Mat());
}



// ############### HORIZON DETECTION CALLS ####################

JNIEXPORT jlong JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeInitDetectHorizon
(JNIEnv* jenv, jclass)
{
	jlong result = 0;

	try
	{
		result = (jlong)new HorizonDetection();
	}
	catch(cv::Exception& e)
	{
		LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if(!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	}
	catch (...)
	{
		LOGD("nativeCreateObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeCreateObject()");
		return 0;
	}
	return result;
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeStopDetectHorizon
(JNIEnv* jenv, jclass, jlong thiz)
{
	HorizonDetection* horizon = (HorizonDetection*)thiz;
	delete horizon;
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectColorEdgeHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates)
{
	if(thiz != 0) {
		HorizonDetection* horizon = (HorizonDetection*)thiz;
		Mat* data = (Mat*) data_;
		Mat* canny = (Mat*) canny_;
		jsize ret_size = jenv->GetArrayLength(coordinates);
		int* ret = (int*)calloc(6*N_HORIZON_RESULTS, sizeof(int));
		horizon->horizonEdgeColorDetection(data, canny, ret);
		jenv->SetIntArrayRegion(coordinates, 0, ret_size, ret);
		free(ret);
	}
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectColorHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates)
{
	if(thiz != 0) {
		HorizonDetection* horizon = (HorizonDetection*)thiz;
		Mat* data = (Mat*) data_;
		Mat* canny = (Mat*) canny_;
		jsize ret_size = jenv->GetArrayLength(coordinates);
		int* ret = (int*)calloc(6*N_HORIZON_RESULTS, sizeof(int));
		horizon->horizonColorDetection(data, canny, ret, false);
		jenv->SetIntArrayRegion(coordinates, 0, ret_size, ret);
		free(ret);
	}
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectEdgeHorizon
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jlong canny_, jintArray coordinates)
{
	if(thiz != 0) {
		HorizonDetection* horizon = (HorizonDetection*)thiz;
		Mat* data = (Mat*) data_;
		Mat* canny = (Mat*) canny_;
		jsize ret_size = jenv->GetArrayLength(coordinates);
		int* ret = (int*)calloc(6*N_HORIZON_RESULTS, sizeof(int));
		horizon->horizonEdgeDetection(data, canny, ret, false);
		jenv->SetIntArrayRegion(coordinates, 0, ret_size, ret);
		free(ret);
	}
}

// ############### HORIZON DETECTION CALLS ####################


// ############### MAJOR LINES CALLS ####################

JNIEXPORT jlong JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeInitMainLinesDetection
(JNIEnv* jenv, jclass) {
	jlong result = 0;

	try
	{
		result = (jlong)new MainLinesDetection();
	}
	catch(cv::Exception& e)
	{
		LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if(!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	}
	catch (...)
	{
		LOGD("nativeCreateObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeCreateObject()");
		return 0;
	}
	return result;
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeStopMainLinesDetection
(JNIEnv* jenv, jclass, jlong thiz){
	if(thiz != 0) {
		MainLinesDetection* ml = (MainLinesDetection*)thiz;
		delete ml;
	}
}

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetMainLines
(JNIEnv* jenv, jclass, jlong thiz, jlong data_, jint threshold, jobject list)
{
	jclass cls_arraylist = jenv->GetObjectClass(list);
	jclass cls_int = jenv->FindClass("java/lang/Integer");

	jmethodID int_cstr = jenv->GetMethodID(cls_int, "<init>", "(I)V");
	jmethodID arraylist_add = jenv->GetMethodID(cls_arraylist,"add","(Ljava/lang/Object;)Z");

	//	jint d = 1;
	//	jobject a = jenv->NewObject(cls_int, int_cstr, d);
	//	jboolean result = jenv->CallBooleanMethod(list,arraylist_set,a);
	if(thiz != 0) {
		MainLinesDetection* ml = (MainLinesDetection*)thiz;
		Mat* data = (Mat*) data_;
		vector< pair< pair<int, int>, pair<int, int> > > lines;
		lines = ml->getLines(*data, threshold);

		for(int i = 0; i < lines.size(); i++) {
			pair<int, int> p0 = lines[i].first;
			pair<int, int> p1 = lines[i].second;

			jobject x0 = jenv->NewObject(cls_int, int_cstr, p0.first);
			jobject y0 = jenv->NewObject(cls_int, int_cstr, p0.second);
			jobject x1 = jenv->NewObject(cls_int, int_cstr, p1.first);
			jobject y1 = jenv->NewObject(cls_int, int_cstr, p1.second);

			jenv->CallBooleanMethod(list, arraylist_add, x0);
			jenv->CallBooleanMethod(list, arraylist_add, y0);
			jenv->CallBooleanMethod(list, arraylist_add, x1);
			jenv->CallBooleanMethod(list, arraylist_add, y1);
		}
	}
}
// ############### MAJOR LINES ALLS ####################

// ############### OBJECT SEGMENTATION CALLS ####################

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetSegmetationMask
(JNIEnv* jenv, jclass, jlong data_, jlong ret_)
{
	Mat* data = (Mat*) data_;
	Mat* ret = (Mat*) ret_;
	Mat data_bgr, tmp;
	cvtColor(*data, data_bgr, CV_YUV420sp2BGR, 3);
	Mat mask;
	ObjectSegmentation::getSegmentationMask(data_bgr, mask);
	cvtColor(mask, tmp, CV_GRAY2BGRA);

	uchar* pixels = (uchar*)tmp.data;
	uchar* mask_pixels = (uchar*)mask.data;
	for(int i = 0; i < tmp.rows; i++) {
		for(int j = 0; j < tmp.cols; j++) {
			if(mask_pixels[(i*mask.cols + j)] == 0) {
				pixels[i*tmp.cols*4 + j*4 + 3] = 0;		//A
			} else {
				pixels[i*tmp.cols*4 + j*4] = 0;			//B
				pixels[i*tmp.cols*4 + j*4 + 1] = 255; 	//G
				pixels[i*tmp.cols*4 + j*4 + 2] = 0;		//R
			}
		}
	}
	*ret = tmp;
}

// ############### OBJECT SEGMENTATION CALLS ####################

// ############### BACKGROUND SIMPLICITY CALLS ####################

JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeTestBgSimplicity
(JNIEnv* jenv, jclass, jlong data_, jfloatArray ret) {
	Mat* data = (Mat*) data_;
	Mat data_rgb;
	cvtColor(*data, data_rgb, CV_YUV420sp2RGB, 3);
	float method1 = BackgroundSimplicity::method1(data_rgb);
	float method2 = BackgroundSimplicity::method2(data_rgb);
	float method3 = BackgroundSimplicity::method3(data_rgb);

	float results[3] = {method1, method2, method3};
	jenv->SetFloatArrayRegion(ret, 0, 3, results);
}

// ############### BACKGROUND SIMPLICITY CALLS ####################

// ############### HUE COUNT CALLS ####################

JNIEXPORT jint JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetHueCount
(JNIEnv* jenv, jclass, jlong data_) {
	Mat* data = (Mat*) data_;
	Mat data_rgb;
	cvtColor(*data, data_rgb, CV_YUV420sp2RGB, 3);
	int ret = HueCounter::getHueCount(data_rgb);
	return ret;
}

// ############### HUE COUNT CALLS ####################

// ############### IMAGE BALANCE CALLS ####################

JNIEXPORT jint JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetImageBalance
(JNIEnv* jenv, jclass, jlong data_, jlong output_) {
	Mat* data = (Mat*) data_;
	Mat* output = (Mat*) output_;

	Mat data_rgb;
	cvtColor(*data, data_rgb, CV_YUV420sp2BGR, 3);
	int balance = ImageBalance::getImageBalance(data_rgb, *output);
	return balance;
}
// ############### IMAGE BALANCE CALLS ####################

// ############### COLOR TEMPLATE CALLS ####################
JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeGetColorTemplate
(JNIEnv* jenv, jclass, jlong data_, jobject list)
{
	jclass cls_arraylist = jenv->GetObjectClass(list);
	jclass cls_int = jenv->FindClass("java/lang/Integer");

	jmethodID int_cstr = jenv->GetMethodID(cls_int, "<init>", "(I)V");
	jmethodID arraylist_add = jenv->GetMethodID(cls_arraylist,"add","(Ljava/lang/Object;)Z");

	Mat* data = (Mat*) data_;
	Mat data_rgb;
	cvtColor(*data, data_rgb, CV_YUV420sp2BGR, 3);

	vector<int> bins = ColorTemplate::getTemplate(data_rgb);

	for(int i = 0; i < bins.size(); i++) {
		jobject bin = jenv->NewObject(cls_int, int_cstr, bins[i]);
		jenv->CallBooleanMethod(list, arraylist_add, bin);
	}
}
// ############### COLOR TEMPLATE CALLS ####################

