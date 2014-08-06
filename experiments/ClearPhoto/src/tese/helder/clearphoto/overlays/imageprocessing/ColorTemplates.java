package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
import java.util.Arrays;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.OverlayType;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;
import android.view.View.OnClickListener;

public class ColorTemplates extends ImageProcessingOv{
	private static int MARGIN = 20;
	
	private Mat data;
	private int previewWidth, previewHeight;
	private Bitmap wheel;
	private int wheel_width, wheel_height;
	
	public ColorTemplates(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		
		BitmapFactory.Options dimensions = new BitmapFactory.Options(); 
		dimensions.inJustDecodeBounds = true;
		this.wheel = BitmapFactory.decodeResource(getResources(), R.drawable.color_template, dimensions);
		this.wheel_height = dimensions.outHeight;
		this.wheel_width =  dimensions.outWidth;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		canvas.drawBitmap(wheel, MARGIN, height-wheel_height-MARGIN, null);
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ArrayList<Integer> bins = ImageProcessing.getColorTemplate(data);
		Log.w("asds",bins.toString());
		invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.COLOR_TEMPLATE);
				} else {
					parent.addOverlay(OverlayType.COLOR_TEMPLATE);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}
}
