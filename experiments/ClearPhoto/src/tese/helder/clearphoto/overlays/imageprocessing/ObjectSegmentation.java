package tese.helder.clearphoto.overlays.imageprocessing;

import java.io.ByteArrayOutputStream;
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
import android.graphics.BitmapFactory;
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
	private Bitmap b;
	public ObjectSegmentation(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.output = new Mat(previewHeight, previewWidth, CvType.CV_8UC4);
		overlay = new Paint();
		overlay.setAlpha(100);
		
//		b = BitmapFactory.decodeResource(getResources(), R.raw.example);
//		ByteArrayOutputStream stream = new ByteArrayOutputStream();
//		b.compress(Bitmap.CompressFormat.JPEG, 100, stream);
//		byte[] byteArray = stream.toByteArray();
//		
//		data.put(0, 0, getNV21(previewWidth, previewHeight, b));
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(bitmap != null) {
//			canvas.drawBitmap(b, 0, 0, null);
			canvas.drawBitmap(bitmap, 0, 0, overlay);
		}
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
		Bitmap tmp = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888);
		Utils.matToBitmap(output, tmp);
		bitmap = Bitmap.createScaledBitmap(tmp, width, height, false);
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
	 // untested function
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
}
