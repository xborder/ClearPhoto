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
import android.graphics.Point;
import android.graphics.Rect;
import android.nfc.tech.NfcB;
import android.util.Log;
import android.util.Pair;

public class ColorHistogram extends ImageProcessingOv {

	private static int BAR_GAP = 30;
	private static int BAR_THICKNESS = 20;
	private static int LABELS_GAP = 10;
	private static int MAX_EXPOSURE_LINE_HEIGHT = BAR_THICKNESS+200;
	private static int NUMBINS = 256;

	private Paint rBarColor, gBarColor, bBarColor, grayBarColor, outlineColor, infoColor;
	private Mat rHist, gHist, bHist;
	private Mat grayHist;
	private Mat data;
	private double graySpreadRatio, rgbSpreadRatio; 
	private double exposureSpreadRatio;
	private int previewHeight, previewWidth;
	private boolean grayProcessed, rgbProcessed;

	private Point grayHistLT, grayHistRB;
	private Point rHistLT, rHistRB, gHistLT, gHistRB, bHistLT, bHistRB;
	
	public ColorHistogram(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);

		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.rHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		this.gHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		this.bHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);
		this.grayHist = new Mat(NUMBINS, 1, CvType.CV_8UC1);

		this.grayProcessed = false;
		this.rgbProcessed = false;
		
		this.exposureSpreadRatio = MAX_EXPOSURE_LINE_HEIGHT/(double)932025;//(BINS_MAX_VALUE*NUMBINS/2);
		
		this.grayHistLT = new Point(0, height/3);
		this.grayHistRB = new Point(BAR_THICKNESS, height/3 * 2);
		int grayBarWidth = grayHistRB.y - grayHistLT.y;
		this.graySpreadRatio = grayBarWidth/(double)NUMBINS;

		this.rHistLT = new Point(BAR_GAP*2, height - BAR_THICKNESS);
		this.rHistRB = new Point(width/3 - BAR_GAP, height);
		
		this.gHistLT = new Point(width/3 + BAR_GAP, height - BAR_THICKNESS);
		this.gHistRB = new Point(width/3*2 - BAR_GAP, height);
		
		this.bHistLT = new Point(width/3*2 + BAR_GAP, height - BAR_THICKNESS);
		this.bHistRB = new Point(width - BAR_GAP*2, height);
		int rgbBarWidth = rHistRB.x - rHistLT.x;
		this.rgbSpreadRatio = rgbBarWidth/(double)NUMBINS;
		
		grayBarColor = new Paint();
		grayBarColor.setStyle(Paint.Style.FILL);
		grayBarColor.setColor(Color.GRAY);
		
		rBarColor = new Paint();
		rBarColor.setStyle(Paint.Style.FILL);
		rBarColor.setColor(Color.RED);

		gBarColor = new Paint();
		gBarColor.setStyle(Paint.Style.FILL);
		gBarColor.setColor(Color.GREEN);

		bBarColor = new Paint();
		bBarColor.setStyle(Paint.Style.FILL);
		bBarColor.setColor(Color.BLUE);
		
		outlineColor = new Paint();
		outlineColor.setStyle(Paint.Style.STROKE);
		outlineColor.setColor(Color.BLACK);
		
		infoColor = new Paint();
		infoColor.setStyle(Paint.Style.STROKE);
		infoColor.setColor(Color.MAGENTA);
		infoColor.setStrokeWidth(2f);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(grayProcessed) {
			grayProcessed = false;
			drawHistogram(canvas, grayHist, grayHistLT, grayHistRB, grayBarColor, graySpreadRatio, true);
		}
		if(rgbProcessed) {
			rgbProcessed = false;
			drawHistogram(canvas, rHist, rHistLT, rHistRB, rBarColor, rgbSpreadRatio, false);
			drawHistogram(canvas, gHist, gHistLT, gHistRB, gBarColor, rgbSpreadRatio, false);
			drawHistogram(canvas, bHist, bHistLT, bHistRB, bBarColor, rgbSpreadRatio, false);
		}
		drawLimitsText(canvas);
	}

	private void drawLimitsText(Canvas canvas) {
		String lowerText = "0", upperText = "255";
		
		int left = grayHistLT.x, top = grayHistLT.y, right = grayHistRB.x, bottom = grayHistRB.y;
		canvas.drawText(lowerText, right, top - LABELS_GAP,infoColor);		
		canvas.drawText(upperText, right, bottom + LABELS_GAP, infoColor);
		
		left = rHistLT.x; top = rHistLT.y; right = rHistRB.x; bottom = rHistRB.y;
		canvas.drawText(lowerText, left - LABELS_GAP, top,infoColor);		
		canvas.drawText(upperText, right, top, infoColor);

		left = gHistLT.x; top = gHistLT.y; right = gHistRB.x; bottom = gHistRB.y;
		canvas.drawText(lowerText, left - LABELS_GAP, top,infoColor);		
		canvas.drawText(upperText, right, top, infoColor);

		left = bHistLT.x; top = bHistLT.y; right = bHistRB.x; bottom = bHistRB.y;
		canvas.drawText(lowerText, left - LABELS_GAP, top,infoColor);		
		canvas.drawText(upperText, right, top, infoColor);
	}


	private void drawHistogram(Canvas canvas, Mat hist, Point histLT, Point histRB, Paint fill, double spreadRatio, boolean vertical) {
		Pair<Integer, Integer> barLimits = getHistogramLimits(hist);
		int firstLimit = barLimits.first, lastLimit = barLimits.second;
		
		int left = histLT.x, top = histLT.y, right = histRB.x, bottom = histRB.y;
		
		Rect bar;
		if(vertical) {
			bar = new Rect(left, (int)(top + firstLimit * spreadRatio), right, (int)(top + lastLimit * spreadRatio));
		} else {
			bar = new Rect((int)(left + firstLimit * spreadRatio), top, (int)(left + lastLimit * spreadRatio), bottom);
		}
		canvas.drawRect(bar, fill);
	    canvas.drawRect(bar, outlineColor);
	    
		float lowerWeight = 0.0f, upperWeight = 0.0f;
		for(int i = 0; i < hist.rows()/3; i ++) {
			lowerWeight += hist.get(i, 0)[0] * (NUMBINS/3-i);
			upperWeight += hist.get(hist.rows() - 1 - i, 0)[0] * (NUMBINS/3-i);
		}

		//draw limits
		if(vertical) {
			canvas.drawLine(left, top,		(int)(right + lowerWeight * exposureSpreadRatio), top, infoColor);
			canvas.drawLine(left, bottom, 	(int)(right + upperWeight * exposureSpreadRatio), bottom, infoColor);	
		} else {
			canvas.drawLine(left, bottom, left, (int)(top - lowerWeight * exposureSpreadRatio), infoColor);
			canvas.drawLine(right, bottom, right, (int)(top - upperWeight * exposureSpreadRatio), infoColor);
		}
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
	
	private Pair<Integer,Integer> getHistogramLimits(Mat hist) {
		ArrayList<Integer> nonNegativeValues = new ArrayList<Integer>();
		for(int i = 0; i < grayHist.rows(); i++) {
			double val = grayHist.get(i,0)[0];
			if(val > 0) {
				nonNegativeValues.add(i);
			}
		}
		Collections.sort(nonNegativeValues);
		int firstLimit = nonNegativeValues.get(0), lastLimit = nonNegativeValues.get(nonNegativeValues.size()-1);
		return new Pair<Integer, Integer>(firstLimit, lastLimit);
	}
	
}
