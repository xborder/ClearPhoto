package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.Arrays;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgproc.Imgproc;

import tese.helder.clearphoto.ImageProcessing;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;

public class ColorHistogram extends ImageProcessingOv {

	private Mat rHist, gHist, bHist;
	private Mat grayHist;
	private Mat data;
	private int grayHist_w, grayHist_h;
	private int rgbHist_w, rgbHist_h;
	private int previewHeight, previewWidth;
	
	public ColorHistogram(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		
		this.grayHist_w = 20; this.grayHist_h = height/3;		//dimensions for gray histogram
		this.rgbHist_w = width/3 - 120; this.rgbHist_h = 20;	//dimensions for rgb histogram
		
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.rHist = new Mat(rgbHist_h, 1, CvType.CV_8UC1);
		this.gHist = new Mat(rgbHist_h, 1, CvType.CV_8UC1);
		this.bHist = new Mat(rgbHist_h, 1, CvType.CV_8UC1);
		this.grayHist = new Mat(grayHist_h, 1, CvType.CV_8UC1);
	}

	@Override
	protected void onDraw(Canvas canvas) {

		Paint detectionColor = new Paint();
		detectionColor.setStyle(Paint.Style.STROKE);
		detectionColor.setColor(Color.BLACK);
		
		MinMaxLocResult c = Core.minMaxLoc(grayHist);

		Log.w(">>>", c.minVal +" "+c.maxVal);
		for(int i = 0; i < grayHist.rows(); i++) {
			double[] val = grayHist.get(i, 0);
			double[] r = rHist.get(i, 0);
			double[] g = gHist.get(i, 0);
			double[] b = bHist.get(i, 0);
			canvas.drawLine(0.0f, (float)i, (float)val[0], (float)i, detectionColor);
		}


		Paint gre = new Paint();
		gre.setStyle(Paint.Style.STROKE);
		gre.setColor(Color.GREEN);
		

		Paint blu = new Paint();
		blu.setStyle(Paint.Style.STROKE);
		blu.setColor(Color.BLUE);
		

		Paint red = new Paint();
		red.setStyle(Paint.Style.STROKE);
		red.setColor(Color.RED);
		for(int i = 0; i < rHist.rows(); i++) {
			double[] r = rHist.get(i, 0);
			double[] g = gHist.get(i, 0);
			double[] b = bHist.get(i, 0);
			canvas.drawLine((float)i, (float)height, (float)i, (float)(height-r[0]), red);
			canvas.drawLine((float)i+300, (float)height, (float)i+300, (float)(height-b[0]), blu);
			canvas.drawLine((float)i+600, (float)height, (float)i+600, (float)(height-g[0]), gre);
		}
		
		
	}
	
	private void drawBarHistogram(Mat hist, int x0, int y0, int x1, int y1, Paint color) {
		
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.getGrayHistogram(data, grayHist, previewWidth, previewHeight);
		ImageProcessing.getRGBHistogram(data, rHist, gHist, bHist, previewWidth, previewHeight);
//		Log.w(">>>>>R", rHist.dump());
//		Log.w(">>>>>G", gHist.dump());
//		Log.w(">>>>>B", bHist.dump());
		this.invalidate();
	}
}
