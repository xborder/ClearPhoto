package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.Arrays;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.View;
import android.view.View.OnClickListener;

public class HorizonDetection extends ImageProcessingOv {
	private static final int FRAMES_INTERVAL = 5;

	private Mat data;
	private int[] horizonCoordinates;
	private int previewWidth, previewHeight;
	private Paint ovColor;
	private float X_RATIO;
	private float Y_RATIO;
	private int frame_count;

	public HorizonDetection(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);

		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.horizonCoordinates = new int[8];

		this.X_RATIO = width/(float)previewWidth;
		this.Y_RATIO = height/(float)previewHeight;

		this.ovColor = new Paint();
		this.ovColor.setStyle(Paint.Style.STROKE);
		this.ovColor.setColor(Color.GREEN);
		this.ovColor.setStrokeWidth(2);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(horizonCoordinates != null) {
			canvas.drawLine(horizonCoordinates[0] * X_RATIO, horizonCoordinates[1] * Y_RATIO, 
					horizonCoordinates[2] * X_RATIO, horizonCoordinates[3] * Y_RATIO, ovColor);
			canvas.drawLine(horizonCoordinates[4] * X_RATIO, horizonCoordinates[5] * Y_RATIO, 
					horizonCoordinates[6] * X_RATIO, horizonCoordinates[7] * Y_RATIO, ovColor);
		}
	}

	@Override
	public void process(byte[] data_) {
		if(frame_count % FRAMES_INTERVAL != 0)
			return;
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.detectHorizon(data, previewWidth, previewHeight, horizonCoordinates);
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
