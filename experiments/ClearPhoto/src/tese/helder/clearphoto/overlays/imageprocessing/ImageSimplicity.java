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

public class ImageSimplicity  extends ImageProcessingOv {
	private Bitmap icon;
	private Mat data;
	private Bitmap bitmap;
	private float[] results;
	private Paint paint;
	private Paint barsPaint;
	public ImageSimplicity(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);

		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.paint = new Paint();
		this.paint.setStyle(Paint.Style.FILL);
		this.paint.setColor(Color.WHITE);
		this.paint.setTextSize(40.0f);

		this.barsPaint = new Paint();
		this.barsPaint.setStyle(Paint.Style.FILL);
		this.barsPaint.setColor(Color.WHITE);
		this.barsPaint.setTextSize(2.0f);
		this.icon = BitmapFactory.decodeResource(getResources(), R.drawable.arrow_icon);
//		bitmap = BitmapFactory.decodeResource(getResources(), R.raw.teste);
//		ByteArrayOutputStream stream = new ByteArrayOutputStream();
//		bitmap.compress(Bitmap.CompressFormat.JPEG, 100, stream);
//		byte[] byteArray = stream.toByteArray();
//
//		data.put(0, 0, getNV21(previewWidth, previewHeight, bitmap));
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(results != null) {
//			canvas.drawText("Method 1: " + results[0] + " (max: 100)", 15, 150, paint);
//			canvas.drawText("Method 2: " + results[1] + " (max: 1)", 15, 200, paint);
//			canvas.drawText("Method 3: " + results[2] + " (max: 1)", 15, 250, paint);

			canvas.drawText("Method 1:", 15, height - 220, paint);
			canvas.drawText("Method 2:", 15, height - 120, paint);
			canvas.drawText("Method 3:", 15, height - 20, paint);
			
			canvas.drawLine(250, height - 220 , 550, height - 220, barsPaint);
			canvas.drawLine(250, height - 120 , 550, height - 120, barsPaint);
			canvas.drawLine(250, height - 20 , 550, height - 20, barsPaint);
			
			Log.w(">>", "results 0 "+ results[0]*300/100);
			Log.w(">>", "results 1 "+ results[1]*300);
			Log.w(">>", "results 2 "+ results[2]*300);
			canvas.drawBitmap(icon, 250 + results[0]*300/100 - 15, height - 220 - 30, null);
			canvas.drawBitmap(icon, 250 + results[1]*300 - 15, height - 120 - 30, null);
			canvas.drawBitmap(icon, 250 + results[2]*300 - 15, height - 20 - 30, null);
		}
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		results = ImageProcessing.getBgSimplicity(data);
//		Log.e("adasd", Arrays.toString(results));
		this.invalidate();
	}


	byte [] getNV21(int inputWidth, int inputHeight, Bitmap scaled) {

		int [] argb = new int[inputWidth * inputHeight];

		scaled.getPixels(argb, 0, inputWidth, 0, 0, inputWidth, inputHeight);

		byte [] yuv = new byte[inputWidth*inputHeight*3/2];
		encodeYUV420SP(yuv, argb, inputWidth, inputHeight);


		return yuv;
	}

	void encodeYUV420SP(byte[] yuv420sp, int[] argb, int width, int height) {
		final int frameSize = width * height;

		int yIndex = 0;
		int uvIndex = frameSize;

		int a, R, G, B, Y, U, V;
		int index = 0;
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				a = (argb[index] & 0xff000000) >> 24; // a is not used obviously
				R = (argb[index] & 0xff0000) >> 16;
				G = (argb[index] & 0xff00) >> 8;
				B = (argb[index] & 0xff) >> 0;

				// well known RGB to YUV algorithm
				Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
				U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
				V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

				// NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
				//    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
				//    pixel AND every other scanline.
				yuv420sp[yIndex++] = (byte) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
				if (j % 2 == 0 && index % 2 == 0) { 
					yuv420sp[uvIndex++] = (byte)((V<0) ? 0 : ((V > 255) ? 255 : V));
					yuv420sp[uvIndex++] = (byte)((U<0) ? 0 : ((U > 255) ? 255 : U));
				}

				index ++;
			}
		}
	}
	
	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.BACKGROUND_SIMPLICITY);
				} else {
					parent.addOverlay(OverlayType.BACKGROUND_SIMPLICITY);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}
}
