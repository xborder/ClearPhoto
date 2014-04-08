package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.PriorityQueue;

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
import android.graphics.Rect;
import android.nfc.tech.NfcB;
import android.util.Log;

public class ColorHistogram extends ImageProcessingOv {
	
	private static int GAP = 30;
	private static int BAR_THICKNESS = 20;
	private static int MAX_EXPOSURE_SIZE = BAR_THICKNESS+200;
	private static int NUMBINS = 256;
	private static int BINS_MAX_VALUE = NUMBINS;
	
	private Mat rHist, gHist, bHist;
	private Mat grayHist;
	private Mat data;
	private int grayHist_w, grayHist_h;
	private double grayHist_ratio, grayHistExposure_ratio;
	private int rgbHist_w, rgbHist_h;
	private int previewHeight, previewWidth;
	private boolean grayProcessed, rgbProcessed;
	
	public ColorHistogram(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		
		this.grayHist_w = BAR_THICKNESS; this.grayHist_h = height/3;		//dimensions for gray histogram
		this.grayHist_ratio = grayHist_h/(double)NUMBINS;
		this.grayHistExposure_ratio = MAX_EXPOSURE_SIZE/(double)932025;//(BINS_MAX_VALUE*NUMBINS/2);
		
		this.rgbHist_w = width/3 - GAP; this.rgbHist_h = BAR_THICKNESS;		//dimensions for rgb histogram
		
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.rHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		this.gHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		this.bHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		this.grayHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		
		this.grayProcessed = false;
		this.rgbProcessed = false;
	}

	@Override
	protected void onDraw(Canvas canvas) {

		Paint detectionColor = new Paint();
		detectionColor.setStyle(Paint.Style.STROKE);
		detectionColor.setColor(Color.BLACK);
		
		if(grayProcessed) {
			grayProcessed = false;
			drawGrayHistogram(canvas);
		}
//		for(int i = 0; i < grayHist.rows(); i++) {
//			double[] val = grayHist.get(i, 0);
//			double[] r = rHist.get(i, 0);
//			double[] g = gHist.get(i, 0);
//			double[] b = bHist.get(i, 0);
//			canvas.drawLine(0.0f, (float)i, (float)val[0], (float)i, detectionColor);
//		}
//
//
//		Paint gre = new Paint();
//		gre.setStyle(Paint.Style.STROKE);
//		gre.setColor(Color.GREEN);
//		
//
//		Paint blu = new Paint();
//		blu.setStyle(Paint.Style.STROKE);
//		blu.setColor(Color.BLUE);
//		
//
//		Paint red = new Paint();
//		red.setStyle(Paint.Style.STROKE);
//		red.setColor(Color.RED);
//		for(int i = 0; i < rHist.rows(); i++) {
//			double[] r = rHist.get(i, 0);
//			double[] g = gHist.get(i, 0);
//			double[] b = bHist.get(i, 0);
//			canvas.drawLine((float)i, (float)height, (float)i, (float)(height-r[0]), red);
//			canvas.drawLine((float)i+300, (float)height, (float)i+300, (float)(height-b[0]), blu);
//			canvas.drawLine((float)i+600, (float)height, (float)i+600, (float)(height-g[0]), gre);
//		}
		
		
	}
	
	private void drawGrayHistogram(Canvas canvas) {
		//Limits extraction
		ArrayList<Integer> nonNegativeValues = new ArrayList<Integer>();
		for(int i = 0; i < grayHist.rows(); i++) {
			double val = grayHist.get(i,0)[0];
			if(val > 0) {
				nonNegativeValues.add(i);
			}
		}
		Collections.sort(nonNegativeValues);
		int firstLimit = nonNegativeValues.get(0), lastLimit = nonNegativeValues.get(nonNegativeValues.size()-1);
		
		Paint barColor= new Paint();
		barColor.setStyle(Paint.Style.FILL);
		barColor.setColor(Color.GRAY);
		Rect bar = new Rect(grayHist_w, (int)(grayHist_h + firstLimit*grayHist_ratio), 0, (int)(grayHist_h + lastLimit*grayHist_ratio));
		canvas.drawRect(bar, barColor);
		
		float lowerWeight = 0.0f, upperWeight = 0.0f;
		for(int i = 0; i < grayHist.rows()/3; i ++) {
			lowerWeight += grayHist.get(i, 0)[0] * (NUMBINS/3-i);
			upperWeight += grayHist.get(grayHist.rows() - 1 - i, 0)[0] * (NUMBINS/3-i);
		}

		Paint limits = new Paint();
		limits.setStyle(Paint.Style.STROKE);
		limits.setColor(Color.MAGENTA);
		limits.setStrokeWidth(2f);
		//draw limits
		
//		int lowerExposure = (lowerWeight > upperWeight) ? (int)(lowerWeight - upperWeight): 0;
//		int upperExposure = (lowerWeight < upperWeight) ? (int)(upperWeight - lowerWeight): 0;
		canvas.drawLine(0, grayHist_h*2, (int)(grayHist_w + upperWeight*grayHistExposure_ratio), grayHist_h*2, limits);
		canvas.drawLine(0, grayHist_h, (int)(grayHist_w + lowerWeight*grayHistExposure_ratio), grayHist_h, limits);
		
		canvas.drawText("0", grayHist_w, grayHist_h - BAR_THICKNESS,limits);		
		canvas.drawText("255", grayHist_w, grayHist_h*2 + + BAR_THICKNESS, limits);
		

//		Log.w(">>>", lowerExposure+ " " + upperExposure);
		Log.w(">>>", grayHist.dump());
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.getGrayHistogram(data, grayHist, previewWidth, previewHeight);
		grayProcessed = true;
		
		ImageProcessing.getRGBHistogram(data, rHist, gHist, bHist, previewWidth, previewHeight);
		rgbProcessed = true;
		
		this.invalidate();
	}
}
