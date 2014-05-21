package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

public class HorizonDetection extends ImageProcessingOv {
	private static final int FRAMES_INTERVAL = 5;

	private Mat data, canny;
	private int[] colorCoordinates, edgeCoordinates, bothCoordinates;
	private float X_RATIO;
	private float Y_RATIO;
	private int frame_count;
	private long nativeAddress;
	private Paint edgePaint, colorPaint, bothPaint;
	private Bitmap bmp;
	public HorizonDetection(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);

		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.canny = new Mat(previewHeight, previewWidth, CvType.CV_8UC1);
		
		this.colorCoordinates = new int[18];
		this.edgeCoordinates = new int[18];
		this.bothCoordinates = new int[18];

		this.X_RATIO = width/(float)previewWidth;
		this.Y_RATIO = height/(float)previewHeight;

		this.edgePaint = new Paint();
		this.edgePaint.setStyle(Paint.Style.STROKE);
		this.edgePaint.setColor(Color.GREEN);
		this.edgePaint.setStrokeWidth(2);

		this.colorPaint = new Paint();
		this.colorPaint.setStyle(Paint.Style.STROKE);
		this.colorPaint.setColor(Color.BLUE);
		this.colorPaint.setStrokeWidth(2);

		this.bothPaint = new Paint();
		this.bothPaint.setStyle(Paint.Style.STROKE);
		this.bothPaint.setColor(Color.RED);
		this.bothPaint.setStrokeWidth(2);

		this.nativeAddress = ImageProcessing.initDetectHorizon();
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(colorCoordinates != null) {
			for(int i = 0; i < 16; i+=6)
				canvas.drawLine(colorCoordinates[i+0] * X_RATIO, colorCoordinates[i+1] * Y_RATIO, 
						colorCoordinates[i+2] * X_RATIO, colorCoordinates[i+3] * Y_RATIO, colorPaint);
		}
		if(edgeCoordinates != null) {
			for(int i = 0; i < 16; i+=6)
				canvas.drawLine(edgeCoordinates[i+0] * X_RATIO, edgeCoordinates[i+1] * Y_RATIO, 
						edgeCoordinates[i+2] * X_RATIO, edgeCoordinates[i+3] * Y_RATIO, edgePaint);
		}
		if(bothCoordinates != null) {
			for(int i = 0; i < 16; i+=6)
				canvas.drawLine(bothCoordinates[i+0] * X_RATIO, bothCoordinates[i+1] * Y_RATIO, 
						bothCoordinates[i+2] * X_RATIO, bothCoordinates[i+3] * Y_RATIO, bothPaint);
		}
		if(bmp != null)
		canvas.drawBitmap(bmp, 0,0,null);
	}

	@Override
	public void process(byte[] data_) {
		frame_count = frame_count++ % FRAMES_INTERVAL;
		if(frame_count != 0)
			return;
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
//		ImageProcessing.detectColorHorizon(nativeAddress, data, canny, colorCoordinates);
//		Log.e(">>>>", "COLOR: \t\t" + Arrays.toString(edgeCoordinates));
//		ImageProcessing.detectEdgeHorizon(nativeAddress, data, canny, edgeCoordinates);
//		Log.e(">>>>", "EDGES: \t\t" + Arrays.toString(edgeCoordinates));
		ImageProcessing.detectColorEdgeHorizon(nativeAddress, data, canny, bothCoordinates);
//		Log.e(">>>>", "COLOREDGES: \t" + Arrays.toString(edgeCoordinates));

		bmp = Bitmap.createBitmap(canny.cols(), canny.rows(), Bitmap.Config.ARGB_8888);
	    Utils.matToBitmap(canny, bmp);
		this.invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.HORIZON_DETECTION);
				} else {
					parent.addOverlay(OverlayType.HORIZON_DETECTION);
				}
				inflated = !inflated;
			}
		};
	}
}
