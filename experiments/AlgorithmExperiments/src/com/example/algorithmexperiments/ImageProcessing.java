package com.example.algorithmexperiments;

import java.util.HashSet;
import java.util.Map.Entry;
import java.util.Set;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import compgeom.RLineSegment2D;
import compgeom.RPoint2D;
import compgeom.algorithms.BentleyOttmann;

public class ImageProcessing {

	public static void kmeans(Mat rgba) {
		ImageProcessJNIWrapper.kmeans(rgba.getNativeObjAddr());
	}
	
	public static Mat canny_Java(Mat rgba, Mat gray, int threshold1, int threshold2) {
		Mat temp = new Mat(rgba.height(), rgba.width(), CvType.CV_8UC4);
        Imgproc.Canny(gray, temp, 80, 100);
        Imgproc.cvtColor(temp, rgba, Imgproc.COLOR_GRAY2RGBA, 4);	
        return rgba;
	}
	
	public static Mat canny_bilateral_Java(Mat rgba, Mat gray, int threshold1, int threshold2, int diameter, int sigmacolor, int sigmaSpace) {
		Mat temp = new Mat(rgba.height(), rgba.width(), CvType.CV_8UC4);
		Mat grayClone = gray.clone();
	    Imgproc.bilateralFilter(grayClone, gray, diameter, sigmacolor, sigmaSpace);
	    Imgproc.Canny(gray, temp, threshold1, threshold2);
	    Imgproc.cvtColor(temp, rgba, Imgproc.COLOR_GRAY2RGBA, 4);
	    return rgba;
	}
	
    public static void canny_JNI(Mat rgba, Mat gray, int threshold1, int threshold2) {
		ImageProcessJNIWrapper.canny(gray.getNativeObjAddr(), rgba.getNativeObjAddr(), threshold1, threshold2);
	}

    public static Mat hough(Mat rgba, Mat gray) {
    	Mat temp = new Mat(rgba.height(), rgba.width(), CvType.CV_8UC4);
        Mat lines = new Mat(rgba.cols(), rgba.rows(), CvType.CV_8UC4);
    	
    	Imgproc.Canny(gray, temp, 50, 200);
    	Imgproc.HoughLinesP(temp, lines, 1, Math.PI/180, 50, 50, 10);
    	
    	Set<RLineSegment2D> set = new HashSet<RLineSegment2D>();
        for (int x = 0; x < lines.cols(); x++) 
        {
              double[] vec = lines.get(0, x);
              double x1 = vec[0], 
                     y1 = vec[1],
                     x2 = vec[2],
                     y2 = vec[3];
              RLineSegment2D segment = new RLineSegment2D(new RPoint2D((int)x1,(int)y1), new RPoint2D((int)x2, (int)y2));
              set.add(segment);
//              double distance = Math.sqrt(Math.pow(x2-x1, 2)+Math.pow(y2-y1,2));
//              if(distance < 150) {
//            	  continue;
//              }
//              Point start = new Point(x1, y1);
//              Point end = new Point(x2, y2);
//              Log.w(">>>>>", distance+"");
//              Core.line(rgba, start, end, new Scalar(255,0,0), 3);
        }
        Set<Entry<RPoint2D, Set<RLineSegment2D>>> inter = BentleyOttmann.intersectionsMap(set).entrySet();
        for(Entry<RPoint2D, Set<RLineSegment2D>> e : inter) {
        	if(e.getValue().size() > 2) {
        		for(RLineSegment2D r: e.getValue()) {
        			set.remove(r);
        		}
        	}
        }
        for(RLineSegment2D r: set) {
        	Core.line(rgba, new Point(r.p1.x.intValue(), r.p1.y.intValue()), new Point(r.p2.x.intValue(), r.p2.y.intValue()), new Scalar(255,0,0), 3);
        }
        return rgba;
    }
}

class ImageProcessJNIWrapper {

	public static native void canny(long addrGray, long addrRgba, int threshold1, int threshold2);
	public static native void kmeans(long addrRgba);

}