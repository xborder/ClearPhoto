package tese.helder.clearphoto.overlays.imageprocessing;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.grids.Grid;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;

public class FaceDetection extends ImageProcessingOv {

	private static final String TAG = "FaceDetection";

	private float X_RATIO, Y_RATIO;
	private int mAbsoluteFaceSize = 0;
	private float mRelativeFaceSize = 0.2f;

	private Mat gray;

	private Activity act;
	private Grid grid;
	private CascadeClassifier mJavaDetector;
	private File mCascadeFile;
	private Rect[] facesArray;
	private Paint detectionColor;

	public byte[] frame;

	public FaceDetection(Activity act, Grid grid, int width, int height, int frameWidth, int frameHeight) {
		this(act.getBaseContext(), width, height);
		this.grid = grid;
		this.act = act;

		this.detectionColor = new Paint();
		this.detectionColor.setStyle(Paint.Style.STROKE);
		this.detectionColor.setColor(Color.GREEN);
		this.detectionColor.setStrokeWidth(5);

		this.X_RATIO = frameWidth/(float)width;
		this.Y_RATIO = frameHeight/(float)height;

		this.gray = new Mat(height, width, CvType.CV_8UC1);

		if (mAbsoluteFaceSize == 0) {
			int h = gray.rows();
			if (Math.round(h * mRelativeFaceSize) > 0) {
				mAbsoluteFaceSize = Math.round(h * mRelativeFaceSize);
			}
		}

		loadCascadeFile();
	}

	private FaceDetection(Context context, int width, int height) {
		super(context, width, height);
	}

	private void loadCascadeFile() {
		try {
			// load cascade file from application resources
			InputStream is = act.getResources().openRawResource(R.raw.lbpcascade_frontalface);
			File cascadeDir = act.getDir("cascade", Context.MODE_PRIVATE);
			mCascadeFile = new File(cascadeDir, "lbpcascade_frontalface.xml");
			FileOutputStream os = new FileOutputStream(mCascadeFile);

			byte[] buffer = new byte[is.available()];
			int bytesRead;
			while ((bytesRead = is.read(buffer)) != -1) {
				os.write(buffer, 0, bytesRead);
			}
			is.close();
			os.close();

			mJavaDetector = new CascadeClassifier(mCascadeFile.getAbsolutePath());
			if (mJavaDetector.empty()) {
				Log.e(TAG, "Failed to load cascade classifier");
				mJavaDetector = null;
			} else
				Log.i(TAG, "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());

			cascadeDir.delete();

		} catch (IOException e) {
			e.printStackTrace();
			Log.e(">>>", "Failed to load cascade. Exception thrown: " + e);
		}
	}

	//YUV contains grayscale image in the first w*h bytes
	@Override
	protected void onDraw(Canvas canvas) {
		if(frame == null)
			return;

		if(facesArray.length > 0) {
			drawFacesDetected(canvas);
		}
	}
	
	private void drawFacesDetected(Canvas canvas) {
		boolean drawConnection = facesArray.length == 3;
		float x0, y0, x1, y1;
		Rect first = facesArray[0];
		Rect prev = facesArray[0];
		Rect cur = null;
		for(int i = 0; i < 3 && facesArray != null && i < facesArray.length; i++) {
			cur = facesArray[i];
			canvas.drawRect((float)cur.tl().x * X_RATIO, (float)cur.tl().y * Y_RATIO, (float)cur.br().x * X_RATIO, (float)cur.br().y * Y_RATIO, detectionColor);
			if(drawConnection) { //draw connecting lines between faces
				x0 = (float) (cur.tl().x+cur.width/2) * X_RATIO;
				y0 = (float) (cur.tl().y+cur.height/2) * Y_RATIO;
				x1 = (float) (prev.tl().x+prev.width/2) * X_RATIO;
				y1 = (float) (prev.tl().y+prev.height/2) * Y_RATIO;
				canvas.drawLine(x0,y0,x1,y1,detectionColor);
			}
			prev = cur;
		}
		if(drawConnection) { 
			//draw the last connecting line between faces
			x0 = (float) (first.tl().x+first.width/2) * X_RATIO;
			y0 = (float) (first.tl().y+first.height/2) * Y_RATIO;
			x1 = (float) (prev.tl().x+prev.width/2) * X_RATIO;
			y1 = (float) (prev.tl().y+prev.height/2) * Y_RATIO;
			canvas.drawLine(x0,y0,x1,y1,detectionColor);
		}
	}
	
	@Override
	public void process(byte[] data) {
		frame = data;
		gray.put(0, 0, Arrays.copyOfRange(data, 0, width*height));

		MatOfRect faces = new MatOfRect();
		if (mJavaDetector != null) {
			mJavaDetector.detectMultiScale(gray, faces, 1.1, 2, 2, new Size(mAbsoluteFaceSize, mAbsoluteFaceSize), new Size());
			facesArray = faces.toArray();
			float[] facesCenter = new float[facesArray.length * 2];
			for(int i = 0, j = 0; i < facesArray.length; i++) {
				facesCenter[j++] = (float) (facesArray[i].tl().x + facesArray[i].width/2) * X_RATIO;
				facesCenter[j++] = (float) (facesArray[i].tl().y + facesArray[i].height/2) * Y_RATIO;
			}

			if(facesArray.length > 0 && facesArray.length <= 3) {
				float[] nearest = ImageProcessing.nearestPoint(grid.getPowerPoints(), facesCenter);
				if(grid != null) {
					grid.highligthPowerPoint(nearest);
				}
			} else {
				grid.highligthPowerPoint(null);
			}
			this.invalidate();
		}
	}

	public static void decodeYUV(int[] out, byte[] fg, int width, int height) throws NullPointerException, IllegalArgumentException { 
		final int sz = width * height; 
		if(out == null) throw new NullPointerException("buffer 'out' is null"); 
		if(out.length < sz) throw new IllegalArgumentException("buffer 'out' size " + out.length + " < minimum " + sz); 
		if(fg == null) throw new NullPointerException("buffer 'fg' is null"); 
		if(fg.length < sz) throw new IllegalArgumentException("buffer 'fg' size " + fg.length + " < minimum " + sz * 3/ 2); 
		int i, j; 
		int Y, Cr = 0, Cb = 0; 
		for(j = 0; j < height; j++) { 
			int pixPtr = j * width; 
			final int jDiv2 = j >> 1; 
		for(i = 0; i < width; i++) { 
			Y = fg[pixPtr]; if(Y < 0) Y += 255; 
			if((i & 0x1) != 1) { 
				final int cOff = sz + jDiv2 * width + (i >> 1) * 2; 
				Cb = fg[cOff]; 
				if(Cb < 0) Cb += 127; else Cb -= 128; 
				Cr = fg[cOff + 1]; 
				if(Cr < 0) Cr += 127; else Cr -= 128; 
			} 
			int R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5); 
			if(R < 0) R = 0; else if(R > 255) R = 255; 
			int G = Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 
			3) + (Cr >> 4) + (Cr >> 5); 
			if(G < 0) G = 0; else if(G > 255) G = 255; 
			int B = Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6); 
			if(B < 0) B = 0; else if(B > 255) B = 255; 
			out[pixPtr++] = 0xff000000 + (B << 16) + (G << 8) + R; 
		} 
		}
	}


	//	static public void decodeYUV420SP(int[] rgb, byte[] yuv420sp, int width, int height) {
	//		final int frameSize = width * height;
	//
	//		for (int j = 0, yp = 0; j < height; j++) {
	//			int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
	//			for (int i = 0; i < width; i++, yp++) {
	//				int y = (0xff & ((int) yuv420sp[yp])) - 16;
	//				if (y < 0) y = 0;
	//				if ((i & 1) == 0) {
	//					v = (0xff & yuv420sp[uvp++]) - 128;
	//					u = (0xff & yuv420sp[uvp++]) - 128;
	//				}
	//
	//				int y1192 = 1192 * y;
	//				int r = (y1192 + 1634 * v);
	//				int g = (y1192 - 833 * v - 400 * u);
	//				int b = (y1192 + 2066 * u);
	//
	//				if (r < 0) r = 0; else if (r > 262143) r = 262143;
	//				if (g < 0) g = 0; else if (g > 262143) g = 262143;
	//				if (b < 0) b = 0; else if (b > 262143) b = 262143;
	//
	//				rgb[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
	//			}
	//		}
	//	}
	//
	//	static public void decodeYUV420SPGrayscale(int[] rgb, byte[] yuv420sp, int width, int height)
	//	{
	//		final int frameSize = width * height;
	//
	//		for (int pix = 0; pix < frameSize; pix++)
	//		{
	//			int pixVal = (0xff & ((int) yuv420sp[pix])) - 16;
	//			if (pixVal < 0) pixVal = 0;
	//			if (pixVal > 255) pixVal = 255;
	//			rgb[pix] = 0xff000000 | (pixVal << 16) | (pixVal << 8) | pixVal;
	//		} // pix
	//	}
}
