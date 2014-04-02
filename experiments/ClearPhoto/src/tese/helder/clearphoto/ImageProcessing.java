package tese.helder.clearphoto;

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
	
	//Native calls for JNI
	private static native void nativeNearestPoint(float[] points, float[] faces, float[] ret);
	private static native void nativeGetHistograms(long data, long rHist, long gHist, long bHist, long grayHist, int previewW, int previewH);
	private static native void nativeGetRGBHistogram(long data, long rHist, long gHist, long bHist, int previewW, int previewH);
	private static native void nativeGetGrayHistogram(long data, long grayHist, int previewW, int previewH);
}
