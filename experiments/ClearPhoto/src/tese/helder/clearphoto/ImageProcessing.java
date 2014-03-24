package tese.helder.clearphoto;

public class ImageProcessing {

	public static float[] nearestPoint(float[] points, float[] faces) {
		float[] nearest = new float[points.length];
		nativeNearestPoint(points, faces, nearest);
		return nearest;
	}

	private static native void nativeNearestPoint(float[] points, float[] faces, float[] ret);
}
