package tese.helder.clearphoto;

import org.opencv.android.OpenCVLoader;

public class ImageProcessing {

	public static float[] nearestPoint(float[] points, float[] faces) {
		float[] nearest = new float[2];
		nativeNearestPoint(points, faces, nearest);
		return nearest;
	}

	private static native void nativeNearestPoint(float[] points, float[] faces, float[] ret);
}
