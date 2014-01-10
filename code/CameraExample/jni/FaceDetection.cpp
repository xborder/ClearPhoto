#include <FaceDetection.h>
#include <opencv2/core/core.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <string>
#include <vector>

#include <android/log.h>

#define LOG_TAG "FaceDetection"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat)
{
	mat = Mat(v_rect, true);
}

JNIEXPORT jlong JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeCreateObject
(JNIEnv * jenv, jclass, jstring jFileName)
{
	LOGD("helder_tese_cameraexample_nativeCreateObject enter");
	const char* jnamestr = jenv->GetStringUTFChars(jFileName, NULL);
	string stdFileName(jnamestr);
	jlong thiz = 0;

	try
	{
		thiz = (jlong)new  CascadeClassifier();
		if(thiz != 0)
		{
			if(!((CascadeClassifier*)thiz)->load(stdFileName)) {
				LOGD("nativeCreateObject caught exception loading haarscascade: %s", stdFileName.c_str());
			}
		}
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
		jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
		return 0;
	}

	LOGD("helder_tese_cameraexample_nativeCreateObject exit");
	return thiz;
}

JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeDestroyObject
(JNIEnv * jenv, jclass, jlong thiz)
{
	LOGD("helder_tese_cameraexample_nativeDestroyObject enter");
	try
	{
		if(thiz != 0)
		{
			delete (CascadeClassifier*)thiz;
		}
	}
	catch(cv::Exception& e)
	{
		LOGD("nativeestroyObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if(!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	}
	catch (...)
	{
		LOGD("nativeDestroyObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
	LOGD("helder_tese_cameraexample_nativeDestroyObject exit");
}

//JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeStart
//(JNIEnv * jenv, jclass, jlong thiz)
//{
//    LOGD("helder_tese_cameraexample_nativeStart enter");
//    try
//    {
////        ((CascadeClassifier*)thiz)->run();
//    }
//    catch(cv::Exception& e)
//    {
//        LOGD("nativeStart caught cv::Exception: %s", e.what());
//        jclass je = jenv->FindClass("org/opencv/core/CvException");
//        if(!je)
//            je = jenv->FindClass("java/lang/Exception");
//        jenv->ThrowNew(je, e.what());
//    }
//    catch (...)
//    {
//        LOGD("nativeStart caught unknown exception");
//        jclass je = jenv->FindClass("java/lang/Exception");
//        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
//    }
//    LOGD("helder_tese_cameraexample_nativeStart exit");
//}

//JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeStop
//(JNIEnv * jenv, jclass, jlong thiz)
//{
//    LOGD("helder_tese_cameraexample_nativeStop enter");
//    try
//    {
////        ((DetectionBasedTracker*)thiz)->stop();
//    }
//    catch(cv::Exception& e)
//    {
//        LOGD("nativeStop caught cv::Exception: %s", e.what());
//        jclass je = jenv->FindClass("org/opencv/core/CvException");
//        if(!je)
//            je = jenv->FindClass("java/lang/Exception");
//        jenv->ThrowNew(je, e.what());
//    }
//    catch (...)
//    {
//        LOGD("nativeStop caught unknown exception");
//        jclass je = jenv->FindClass("java/lang/Exception");
//        jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
//    }
//    LOGD("helder_tese_cameraexample_nativeStop exit");
//}

JNIEXPORT void JNICALL Java_helder_tese_cameraexample_FaceDetection_nativeDetect
(JNIEnv * jenv, jclass, jlong thiz, jint pw, jint ph, jbyteArray image)
{
	//ver https://code.ros.org/trac/opencv/browser/trunk/opencv/samples/android/tutorial-3-native/jni/jni_part.cpp?rev=6344
	try
	{

		uchar* rgb = (uchar*)YUVtoRBG(jenv, image, pw, ph);

		Mat frame(ph, pw, CV_8UC3, rgb);
		Mat frame_gray(ph, pw, CV_8UC1, rgb);

		cvtColor(frame, frame_gray,CV_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		vector<Rect> faces;

		((CascadeClassifier*)thiz)->detectMultiScale( frame_gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING, Size(1, 1) );
		LOGD("faces found: %d", faces.size());
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
		LOGD("nativeDetect caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
}

int* YUVtoRBG(JNIEnv * env, jbyteArray yuv420sp, jint width, jint height)
{
	int* rgbData;
	int rgbDataSize = 0;
	int sz;
	int i;
	int j;
	int Y;
	int Cr = 0;
	int Cb = 0;
	int pixPtr = 0;
	int jDiv2 = 0;
	int R = 0;
	int G = 0;
	int B = 0;
	int cOff;
	int w = width;
	int h = height;
	sz = w * h;

	jboolean isCopy;
	jbyte* yuv = env->GetByteArrayElements(yuv420sp, &isCopy);
	if(rgbDataSize < sz) {
		int tmp[sz];
		rgbData = &tmp[0];
		rgbDataSize = sz;
	}

	for(j = 0; j < h; j++) {
		pixPtr = j * w;
		jDiv2 = j >> 1;
		for(i = 0; i < w; i++) {
			Y = yuv[pixPtr];
			if(Y < 0) Y += 255;
			if((i & 0x1) != 1) {
				cOff = sz + jDiv2 * w + (i >> 1) * 2;
				Cb = yuv[cOff];
				if(Cb < 0) Cb += 127; else Cb -= 128;
				Cr = yuv[cOff + 1];
				if(Cr < 0) Cr += 127; else Cr -= 128;
			}
			R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5);//1.406*~1.403
			if(R < 0) R = 0; else if(R > 255) R = 255;
			G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5);//
			if(G < 0) G = 0; else if(G > 255) G = 255;
			B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6);//1.765~1.770
			if(B < 0) B = 0; else if(B > 255) B = 255;
			rgbData[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R;
		}
	}
	//env->SetIntArrayRegion(rgb, 0, sz, ( jint * ) &rgbData[0] );
	env->ReleaseByteArrayElements(yuv420sp, yuv, JNI_ABORT);
	return rgbData;
}
