package tese.helder.clearphoto.overlays.imageprocessing;

import java.io.ByteArrayOutputStream;
import java.util.Arrays;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;
import android.view.View.OnClickListener;

public class HueCount  extends ImageProcessingOv {

	private Mat data;
	private int count;
	private Paint paint, secondaryPaint;

	public HueCount(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);

		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.paint = new Paint();
		this.paint.setStyle(Paint.Style.FILL);
		this.paint.setColor(Color.WHITE);
		this.paint.setTextSize(100.0f);
		
		this.secondaryPaint = new Paint();
		this.secondaryPaint.setStyle(Paint.Style.FILL);
		this.secondaryPaint.setColor(Color.WHITE);
		this.secondaryPaint.setTextSize(40.0f);
	}

	@Override
	protected void onDraw(Canvas canvas) {
//		canvas.drawText("Hue Count: " + count + " (max: 20)", 15, 150, paint);
		if(count/10 == 0) {
			canvas.drawText(""+count, 50, 150, paint);
		} else {
			canvas.drawText(""+count, 15, 150, paint);	
		}
		canvas.drawText("/", 130, 150, secondaryPaint);
		canvas.drawText("12", 150, 150, secondaryPaint);
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		count = ImageProcessing.getHueCount(data);
		this.invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.HUE_COUNT);
				} else {
					parent.addOverlay(OverlayType.HUE_COUNT);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}
}
