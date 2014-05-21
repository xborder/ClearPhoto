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
	Mat rgba = Mat(height, width, CV_8UC4);
	Mat	hsv = Mat(height, width, CV_8UC3);

	cvtColor(*data, rgba, CV_YUV420p2BGRA);
	cvtColor(rgba, hsv, CV_BGR2HSV);

	vector<Mat> hsv_planes;
	split(hsv, hsv_planes);

	Mat hue = hsv_planes[0];
	int lower_range = 0, upper_range = 0;
	for(int i = 0; i < hue.rows; i++) {
		for(int j = 0; j < hue.cols; j++) {
			uchar val = hue.at<uchar>(i,j);
			for(int r = 0; r < ranges_size-1; r++) {
				lower_range = ranges_[r];
				upper_range = ranges_[r+1];
				if(val >= lower_range && val < upper_range) {
					tmp_array[r] = tmp_array[r] + 1;
					break;
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
/*
JNIEXPORT void JNICALL Java_tese_helder_clearphoto_ImageProcessing_nativeDetectHorizon
(JNIEnv* jenv, jclass, jlong data_, jint width, jint height, jintArray coordinates)
{
	Mat* data = (Mat*) data_;
	int* edge_output = (int*)calloc(6, sizeof(int));
	int* color_output = (int*)calloc(6, sizeof(int));
	horizonEdgeDetection(data, edge_output, NULL);
	horizonColorDetection(data, color_output, width, height, NULL);

	jsize ret_size = jenv->GetArrayLength(coordinates);
	jenv->SetIntArrayRegion(coordinates, 0, ret_size/2, edge_output);
	LOGE(">>>>> %d %d %d %d", edge_output[0], edge_output[1], edge_output[2], edge_output[3]);
	LOGE("----- %d %d %d %d", color_output[0], color_output[1], color_output[2], color_output[3]);
	jenv->SetIntArrayRegion(coordinates, ret_size/2, 4, color_output);
}

void horizonEdgeDetection(Mat* data, int* ret,  double (*intensifyFunction)(int,int,double,double)) {
	vector<Mat> yuv_planes;
	split(*data, yuv_planes);

	Mat luma;
	luma = yuv_planes[0];
	Mat canny_output, blur;

	cv::blur(luma, blur, cv::Size(5,5));

	int thresh = 100;
	int max_thresh = 255;
	Canny( blur, canny_output, thresh, thresh*2, 3 );

	applyHough(canny_output, ret, intensifyFunction);
}

void horizonColorDetection(Mat* data, int* output, int width, int height, double (*intensifyFunction)(int,int,double,double)) {
	vector<Mat> yuv_planes;
	split(*data, yuv_planes );

	Mat sky_filter = Mat::zeros(height, width, CV_8UC1);

	int YM = 210, UM = 150, VM = 100;
	int VARY = 130, VARU = 40, VARV = 40;
	for(int i = 0;i < height;i++){
		double P = pow((double)i / height, 2);
		double positionP = exp(-P);
		for(int j = 0;j < width;j++){
			uchar y = yuv_planes[0].at<uchar>(i,j);
			uchar u = yuv_planes[1].at<uchar>(i,j);
			uchar v = yuv_planes[2].at<uchar>(i,j);
			double A = (y-YM)/VARY;
			double B = (u-UM)/VARU;
			double C = (v-VM)/VARV;
			double Z = (pow(A,2)+ pow(B,2) + pow(C,2));
			double colorP = exp(-Z);

			int t = 0;
			for(int k = -2; k < 2; k++) {
				if(j-k < 0 || j+k >= height)
					continue;
				t += abs(y - yuv_planes[0].at<uchar>(i,j + k));
			}

			int t0 = 10;
			double textureP = (t > t0) ? exp(-0.2 * pow((double)(t - t0), 2)) : 1;

			double Psky = (positionP*colorP*textureP) ;
			sky_filter.at<uchar>(i,j) = (Psky * 100 > SKY_THRESHOLD) ? 255 : 0;
		}
	}

	int thresh = 100;
	int max_thresh = 255;
	Mat canny_output;
	Canny(sky_filter, canny_output, thresh, thresh*2, 3 );

	applyHough(canny_output, output, intensifyFunction);
}


void applyHough(Mat binary_image, int* output, double (*func)(int,int,double,double)) {
	Size img_size(binary_image.cols, binary_image.rows);

	double hough_h = ((sqrt(2.0) * (double)(img_size.height > img_size.width ? img_size.height : img_size.width)) / 2.0); // finding out max rho
	Size acum_size(180, hough_h * 2.0); // -rho -> +rho, 0 -> 180

	double center_x = img_size.width/2;
	double center_y = img_size.height/2;
	int threshold = img_size.width > img_size.height ? img_size.width/4 : img_size.height/4;

	int* acum = (int*)calloc(acum_size.width * acum_size.height, sizeof(int));
	int count = 0, rho_sum = 0, theta_sum = 0;
	vector<int> rhos, thetas;
	for(int y=0; y < img_size.height; y++) {
		for(int x=0 ; x < img_size.width ; x++) {
			if( binary_image.data[ (y * img_size.width) + x] > 250 ) {
				for(int t=0;t<180;t++) {
					double r = ( ((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));
					acum[(int)(round(r + hough_h) * 180.0) + t] ++;
					theta_sum += t; rho_sum += r; count ++;
					rhos.push_back((int)r);
					thetas.push_back(t);
				}
			}
		}
	}

	double rho_deviation = 0.0;
	double theta_deviation = 0.0;

	if(func != NULL) {
		int rhos_avg = rho_sum/count;
		int thetas_avg = theta_sum/count;

		//    rho_deviation = deviation(rhos, rhos_avg);
		//    theta_deviation = deviation(thetas, thetas_avg);
	}

	int max_val = 0, max_rho = 0, max_theta = 0;
	for(int r=0; r < acum_size.height ; r++) {
		for(int t=0; t < acum_size.width; t++) {
			double factor = (func != NULL) ? func(r,t,rho_deviation,theta_deviation) : 1;
			if(acum[(r*acum_size.width) + t] * factor > max_val) {
				max_rho = r;
				max_theta = t;
				max_val = acum[(r*acum_size.width) + t];
			}
		}
	}
	int t = max_theta, r = max_rho;

	int x1, y1, x2, y2;
	x1 = y1 = x2 = y2 = 0;

	if(t >= 45 && t <= 135) {
		//y = (r - x cos(t)) / sin(t)
		x1 = 0;
		y1 = ((double)(r-(acum_size.height/2)) - ((x1 - (img_size.width/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (img_size.height / 2);
		x2 = img_size.width;
		y2 = ((double)(r-(acum_size.height/2)) - ((x2 - (img_size.width/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (img_size.height / 2);
	} else {
		//x = (r - y sin(t)) / cos(t);
		y1 = 0;
		x1 = ((double)(r-(acum_size.height/2)) - ((y1 - (img_size.height/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (img_size.width / 2);
		y2 = img_size.height;
		x2 = ((double)(r-(acum_size.height/2)) - ((y2 - (img_size.height/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (img_size.width / 2);
	}
	output[0] = x1;
	output[1] = y1;
	output[2] = x2;
	output[3] = y2;
	output[4] = t;
	output[5] = r;
}*/

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
	normalize(hist, *output, 0, output->rows, NORM_MINMAX, -1, Mat());
	//    normalize(hist, *output, 0, 1, NORM_MINMAX, -1, Mat());
}



//###########################################################################

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
(JNIEnv* jenv, jclass)
{
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
