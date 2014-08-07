package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.Arrays;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.AsyncTask;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;
import android.view.View.OnClickListener;

public class ImageBalance extends ImageProcessingOv {
	private static int UNBALANCED = 0, VISUAL_TENSION = 1, BALANCED = 2, NONE = -1;
	private Mat data, output;
	private Bitmap toDraw;
	private int previewWidth, previewHeight;
	private Paint overlay;
	private Bitmap balanced, unbalanced, visual_tension;
	private int balanceType = NONE;
	
	public ImageBalance(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.output = new Mat(previewHeight, previewWidth, CvType.CV_8UC4);
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;

		overlay = new Paint();
		overlay.setStyle(Paint.Style.FILL);
		overlay.setColor(Color.BLACK);
		overlay.setAlpha(100);
		
		Resources res = getResources();
		balanced = BitmapFactory.decodeResource(res, R.drawable.ic_balanced);
		unbalanced = BitmapFactory.decodeResource(res, R.drawable.ic_unbalanced);
		visual_tension = BitmapFactory.decodeResource(res, R.drawable.ic_visual_tension);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(toDraw != null)
			canvas.drawBitmap(toDraw, 0, 0, overlay);
	
		if(balanceType == NONE){
			canvas.drawBitmap(balanced, 0, 200, overlay);
			canvas.drawBitmap(unbalanced, 0, 230, overlay);
			canvas.drawBitmap(visual_tension, 0, 260, overlay);
		}
		if(balanceType == UNBALANCED) {
			canvas.drawBitmap(balanced, 0, 200, overlay);
			canvas.drawBitmap(unbalanced, 0, 230, null);
			canvas.drawBitmap(visual_tension, 0, 260, overlay);
		}
		if(balanceType == BALANCED) {
			canvas.drawBitmap(balanced, 0, 200, null);
			canvas.drawBitmap(unbalanced, 0, 230, overlay);
			canvas.drawBitmap(visual_tension, 0, 260, overlay);
		}
		if(balanceType == VISUAL_TENSION) {
			canvas.drawBitmap(balanced, 0, 200, overlay);
			canvas.drawBitmap(unbalanced, 0, 230, overlay);
			canvas.drawBitmap(visual_tension, 0, 260, null);
		}
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		output.setTo(new Scalar(0));
		balanceType = ImageProcessing.getImageBalance(data, output);
		
		Bitmap tmp = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888);
		Utils.matToBitmap(output, tmp);
		toDraw = Bitmap.createScaledBitmap(tmp, width, height, false);
		invalidate();
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
