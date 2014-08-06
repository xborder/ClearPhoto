package tese.helder.clearphoto;

import java.util.ArrayList;

import org.opencv.core.Mat;

public class ImageProcessing {

	public static float[] nearestPoint(float[] points, float[] faces) {
		float[] nearest = new float[faces.length];
		nativeNearestPoint(points, faces, nearest);
		return nearest;
	}

	public static void getHistograms(Mat data, Mat rHist, Mat gHist, Mat bHist, Mat grayHist, int previewW, int previewH) {
		nativeGetHistograms(data.getNativeObjAddr(), rHist.getNativeObjAddr(), gHist.getNativeObjAddr(), 
				bHist.getNativeObjAddr(), grayHist.getNativeObjAddr(), previewW, previewH);
	}

	public static void getRGBHistogram(Mat data, Mat rHist, Mat gHist, Mat bHist, int previewW, int previewH) {
		nativeGetRGBHistogram(data.getNativeObjAddr(), rHist.getNativeObjAddr(), gHist.getNativeObjAddr(), 
				bHist.getNativeObjAddr(), previewH, previewH);
	}

	public static void getGrayHistogram(Mat data, Mat grayHist, int previewW, int previewH) {
		nativeGetGrayHistogram(data.getNativeObjAddr(), grayHist.getNativeObjAddr(), previewW, previewH);
	}

	public static void getHueConcentrarion(Mat data, int[] ret, int[] ranges, int previewW, int previewH) {
		nativeGetHueConcentration(data.getNativeObjAddr(), ret, ranges, previewW, previewH);
	}

	public static int getAvgSaturation(Mat data, int previewW, int previewH) {
		return nativeGetAvgSaturation(data.getNativeObjAddr(), previewW, previewH);
	}


	//Native calls for JNI
	private static native void nativeNearestPoint(float[] points, float[] faces, float[] ret);
	private static native void nativeGetHistograms(long data, long rHist, long gHist, long bHist, long grayHist, int previewW, int previewH);
	private static native void nativeGetRGBHistogram(long data, long rHist, long gHist, long bHist, int previewW, int previewH);
	private static native void nativeGetGrayHistogram(long data, long grayHist, int previewW, int previewH);
	private static native void nativeGetHueConcentration(long data, int[] ret, int[] ranges, int previewW, int previewH);
	private static native int nativeGetAvgSaturation(long data, int previewW, int previewH);


	//============================================================================================
	
	public static long initDetectHorizon() {
		return nativeInitDetectHorizon();
	}
	
	public static void stopDetectHorizon(long nativeAddr) {
		nativeStopDetectHorizon(nativeAddr);
	}
	
	public static void detectColorEdgeHorizon(long nativeAddr, Mat data, Mat canny, int[] ret){
		nativeDetectColorEdgeHorizon(nativeAddr, data.getNativeObjAddr(), canny.getNativeObjAddr(), ret);
	}
	
	public static void detectColorHorizon(long nativeAddr, Mat data, Mat canny, int[] ret) {
		nativeDetectColorHorizon(nativeAddr, data.getNativeObjAddr(), canny.getNativeObjAddr(), ret);
	}
	
	public static void detectEdgeHorizon(long nativeAddr, Mat data, Mat canny, int[] ret) {
		nativeDetectEdgeHorizon(nativeAddr, data.getNativeObjAddr(), canny.getNativeObjAddr(), ret);
	}
	
	private static native long nativeInitDetectHorizon();
	private static native void nativeStopDetectHorizon(long nativeAddr);
	private static native void nativeDetectColorEdgeHorizon(long nativeAddr, long data, long canny, int[] ret);
	private static native void nativeDetectColorHorizon(long nativeAddr, long data, long canny, int[] ret);
	private static native void nativeDetectEdgeHorizon(long nativeAddr, long data, long canny, int[] ret);


	//============================================================================================
	
	public static long initMainLinesDetection() {
		return nativeInitMainLinesDetection();
	}

	public static void stopMainLinesDetection(long nativeAddr) {
		nativeStopDetectHorizon(nativeAddr);
	}
	
	public static void getMainLines(long nativeAddr, Mat data, int threshold, ArrayList<Integer> list) {
		nativeGetMainLines(nativeAddr, data.getNativeObjAddr(), threshold, list);
	}

	private static native long nativeInitMainLinesDetection();
	private static native void nativeStopMainLinesDetection(long nativeAddr);
	private static native void nativeGetMainLines(long nativeAddr, long data, int threshold, ArrayList<Integer> list);

	//============================================================================================

	public static void getSegmetationMask(Mat data, Mat ret) {
		nativeGetSegmetationMask(data.getNativeObjAddr(), ret.getNativeObjAddr());
	}
	
	private static native void nativeGetSegmetationMask(long data, long ret);

	//============================================================================================
	
	public static float[] getBgSimplicity(Mat data) {
		float[] results = new float[3];
		nativeTestBgSimplicity(data.getNativeObjAddr(), results);
		return results;
	}
	
	private static native void nativeTestBgSimplicity(long data, float[] results);
	
	//============================================================================================

	public static int getHueCount(Mat data) {
		return nativeGetHueCount(data.getNativeObjAddr());
	}
	
	private static native int nativeGetHueCount(long data);
	
	//============================================================================================
	
	public static int getImageBalance(Mat data, Mat output) {
		return nativeGetImageBalance(data.getNativeObjAddr(), output.getNativeObjAddr());
	}
	
	private static native int nativeGetImageBalance(long data, long output);
	
	//============================================================================================
	public static ArrayList<Integer> getColorTemplate(Mat data) {
	
		ArrayList<Integer> bins = new ArrayList<Integer>();
		nativeGetColorTemplate(data.getNativeObjAddr(), bins);
		return bins;
	}
	
	private static native void nativeGetColorTemplate(long data, ArrayList<Integer> bins);
	
	//============================================================================================
}
