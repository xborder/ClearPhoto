package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.Arrays;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;
import android.view.View.OnClickListener;

public class ImageBalance extends ImageProcessingOv {
	
	private Mat data, output;
	private Bitmap toDraw;
	private int previewWidth, previewHeight;
	private Paint overlay;
	public ImageBalance(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.output = new Mat(previewHeight, previewWidth, CvType.CV_8UC4);
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		
		overlay = new Paint();
		overlay.setAlpha(100);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(toDraw != null)
			canvas.drawBitmap(toDraw, 0, 0, null);
	}

	@Override
	public void process(byte[] data_) {
		output.setTo(new Scalar(0));
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		int balance = ImageProcessing.getImageBalance(data, output);
		
		Log.w(">>>", balance+"");
		Bitmap tmp = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888);
		Utils.matToBitmap(output, tmp);
		toDraw = Bitmap.createScaledBitmap(tmp, width, height, false);
		this.invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.IMAGE_BALANCE);
				} else {
					parent.addOverlay(OverlayType.IMAGE_BALANCE);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}
}