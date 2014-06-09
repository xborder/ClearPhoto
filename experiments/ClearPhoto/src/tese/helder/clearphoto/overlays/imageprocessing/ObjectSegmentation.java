package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.Arrays;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.OverlayType;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;
import android.view.View.OnClickListener;

public class ObjectSegmentation extends ImageProcessingOv {

	private int previewWidth, previewHeight;
	private Mat data, output;
	private Bitmap bitmap;
	private Paint overlay;
	
	public ObjectSegmentation(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.output = new Mat(previewHeight, previewWidth, CvType.CV_8UC1);
		overlay = new Paint();
		overlay.setStyle(Paint.Style.FILL_AND_STROKE);
		overlay.setColor(Color.RED);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(bitmap != null)
			canvas.drawBitmap(bitmap, 0, 0, overlay);
	}
	
	int frame_count = 0;
	int FRAMES_INTERVAL = 10;
	@Override
	public void process(byte[] data_) {

		frame_count = frame_count++ % FRAMES_INTERVAL;
		if(frame_count != 0)
			return;
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.getSegmetationMask(data,output);
		bitmap = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888);
		Utils.matToBitmap(output, bitmap);
		this.invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.OBJECT_SEGMENTATION);
				} else {
					parent.addOverlay(OverlayType.OBJECT_SEGMENTATION);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}
}
