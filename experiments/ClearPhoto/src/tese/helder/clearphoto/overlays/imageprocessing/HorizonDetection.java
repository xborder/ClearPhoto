package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.Arrays;
import java.util.List;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.MainActivity;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;

public class HorizonDetection extends ImageProcessingOv {
	private static final int FRAMES_INTERVAL = 5;
	private static final int HORIZONTAL_ORIENTATION = 1;
	private static final int VERTICAL_ORIENTATION = 0;
	private static final int NO_ORIENTATION = -1;
	private static final int[] HORIZONTAL_POS = {0,360}, VERTICAL_POS = {275,90};
	private static final int POS_THRESHOLD = 7;

	private Mat data, canny;
	private int[] colorCoordinates, edgeCoordinates, bothCoordinates;
	private float X_RATIO;
	private float Y_RATIO;
	private int frame_count;
	private long nativeAddr;
	private Paint edgePaint, colorPaint, bothPaint;
	private Bitmap bmp;
	private int orientation;

	private SensorManager sm;
	private SensorInformation si;
	private float[] mGravs;
	private float[] mGeoMags;
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
		this.edgePaint.setStrokeWidth(4);

		this.colorPaint = new Paint();
		this.colorPaint.setStyle(Paint.Style.STROKE);
		this.colorPaint.setColor(Color.BLUE);
		this.colorPaint.setStrokeWidth(4);

		this.bothPaint = new Paint();
		this.bothPaint.setStyle(Paint.Style.STROKE);
		this.bothPaint.setColor(Color.RED);
		this.bothPaint.setStrokeWidth(4);

		this.si = new SensorInformation();
		this.sm = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
		this.mGeoMags = new float[3];
		this.mGravs = new float[3];
		this.orientation = NO_ORIENTATION;
	}

	@Override
	protected void onAttachedToWindow() {
		super.onAttachedToWindow();
		nativeAddr = ImageProcessing.initDetectHorizon();

		sm.registerListener(si, sm.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD), SensorManager.SENSOR_DELAY_UI);
		sm.registerListener(si, sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_UI);
	}

	@Override
	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();
		ImageProcessing.stopDetectHorizon(nativeAddr);
		sm.unregisterListener(si);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		drawLines(canvas, bothCoordinates, bothPaint);
		drawLines(canvas, colorCoordinates, colorPaint);
		drawLines(canvas, edgeCoordinates, edgePaint);
//		if(colorCoordinates != null) {
//			for(int i = 0; i < 16; i+=6)
//				canvas.drawLine(colorCoordinates[i+0] * X_RATIO, colorCoordinates[i+1] * Y_RATIO, 
//						colorCoordinates[i+2] * X_RATIO, colorCoordinates[i+3] * Y_RATIO, colorPaint);
//		}
//		if(edgeCoordinates != null) {
//			for(int i = 0; i < 16; i+=6)
//				canvas.drawLine(edgeCoordinates[i+0] * X_RATIO, edgeCoordinates[i+1] * Y_RATIO, 
//						edgeCoordinates[i+2] * X_RATIO, edgeCoordinates[i+3] * Y_RATIO, edgePaint);
//		}
//		if(bothCoordinates != null) {
//			for(int i = 0; i < 16; i+=6)
//				canvas.drawLine(bothCoordinates[i+0] * X_RATIO, bothCoordinates[i+1] * Y_RATIO, 
//						bothCoordinates[i+2] * X_RATIO, bothCoordinates[i+3] * Y_RATIO, bothPaint);
//		}
		if(bmp != null)
			canvas.drawBitmap(bmp, 0,0,null);
	}
	
	private void drawLines(Canvas canvas, int[] lines, Paint color) {
		if(lines != null) {
			for(int i = 0; i < 16; i+=6) {
				double xDiff = lines[i+2] - lines[i+0]; double yDiff = lines[i+3] - lines[i+1]; 
				double angle = Math.toDegrees(Math.atan2(yDiff, xDiff));
				
//				Log.e(">>>", angle + " " + orientation);
				if((orientation == HORIZONTAL_ORIENTATION && angle > -POS_THRESHOLD && angle < POS_THRESHOLD)
						|| (orientation == VERTICAL_ORIENTATION && angle > 90-POS_THRESHOLD && angle < 90 + POS_THRESHOLD)) {
					canvas.drawLine(lines[i+0] * X_RATIO, lines[i+1] * Y_RATIO, 
							lines[i+2] * X_RATIO, lines[i+3] * Y_RATIO, color);
					break;
				} else {
//					Log.e(">>", "not drawn " + lines[i+0] + " " + lines[i+1]+ " " + lines[i+2]+ " " + lines[i+3]);
				}
			}
		}
	}
	@Override
	public void process(byte[] data_) {
		frame_count = frame_count++ % FRAMES_INTERVAL;
		if(frame_count != 0)
			return;
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.detectColorHorizon(nativeAddr, data, canny, colorCoordinates);
		//		Log.e(">>>>", "COLOR: \t\t" + Arrays.toString(edgeCoordinates));
		ImageProcessing.detectEdgeHorizon(nativeAddr, data, canny, edgeCoordinates);
		//		Log.e(">>>>", "EDGES: \t\t" + Arrays.toString(edgeCoordinates));
		ImageProcessing.detectColorEdgeHorizon(nativeAddr, data, canny, bothCoordinates);
		//		Log.e(">>>>", "COLOREDGES: \t" + Arrays.toString(edgeCoordinates));

		//		bmp = Bitmap.createBitmap(canny.cols(), canny.rows(), Bitmap.Config.ARGB_8888);
		//		Utils.matToBitmap(canny, bmp);

		getOrientation();

		this.invalidate();
	}

	private void getOrientation() {
		float[] mRotationM = new float[9];
		float[] mImageM = new float[9];
		float[] mOrientation = new float[3];
		if(mGravs != null && mGeoMags != null) {
			
			SensorManager.getRotationMatrix(mRotationM, mImageM, mGravs, mGeoMags);
			SensorManager.remapCoordinateSystem(mRotationM,
	                SensorManager.AXIS_X,
	                SensorManager.AXIS_Z, mOrientation);
			SensorManager.getOrientation(mRotationM, mOrientation);
			//
			float x = (float) Math.round((Math.toDegrees(mOrientation[1])) *2)/2;
			float y = (float) Math.round((Math.toDegrees(mOrientation[2])) *2)/2;
			float z = (float) Math.round((Math.toDegrees(mOrientation[0])) *2)/2;
			x = (x+360)%360; 
			y = (y+360)%360; 
			z = (z+360)%360; 
			if((x >= HORIZONTAL_POS[0]- POS_THRESHOLD && x <= HORIZONTAL_POS[0] + POS_THRESHOLD)
					|| (x >= HORIZONTAL_POS[1] - POS_THRESHOLD && x <= HORIZONTAL_POS[1] + POS_THRESHOLD)) {
				orientation = HORIZONTAL_ORIENTATION;
//				Log.e(">>>", "horizontal " + x);
			} else if((x >= VERTICAL_POS[0] - POS_THRESHOLD && x <= VERTICAL_POS[0] + POS_THRESHOLD)
					|| (x >= VERTICAL_POS[1] - POS_THRESHOLD && -x <= VERTICAL_POS[1] + POS_THRESHOLD)) {
				orientation = VERTICAL_ORIENTATION;
//				Log.e(">>>", "vertical " + x);
			}
//						Log.e(">>>", "x: "+ x + " y: "+ y + " z: "+ z);
			

			float incl = SensorManager.getInclination(mRotationM);
			Log.w(">>>", ""+(incl*180/Math.PI));
		}		
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
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}

	class SensorInformation implements SensorEventListener {

		public SensorInformation() {}

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
			// TODO Auto-generated method stub

		}

		@Override
		public void onSensorChanged(SensorEvent event) {
			switch (event.sensor.getType()) {
			case Sensor.TYPE_ACCELEROMETER:
				System.arraycopy(event.values, 0, mGravs, 0, 3);
				break;
			case Sensor.TYPE_MAGNETIC_FIELD:
				System.arraycopy(event.values, 0, mGeoMags, 0, 3);
				break;
			default:
				return;
			}
		}

	}
}
