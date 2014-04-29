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

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.OverlayType;
import tese.helder.clearphoto.overlays.grids.Grid;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

public class FaceDetection extends ImageProcessingOv {

	private static final String TAG = "FaceDetection";

	private float X_RATIO, Y_RATIO;
	private int mAbsoluteFaceSize = 0;
	private float mRelativeFaceSize = 0.2f;

	private Mat grayFrame;

	private Activity act;
	private Grid activeGrid;
	private CascadeClassifier detector;
	private File mCascadeFile;
	private Rect[] facesArray;
	private Paint detectionColor;

	public byte[] frame;

	public FaceDetection(Activity act, Grid grid, int width, int height, int frameWidth, int frameHeight) {
		this(act.getBaseContext(), width, height);
		this.activeGrid = grid;
		this.act = act;

		this.detectionColor = new Paint();
		this.detectionColor.setStyle(Paint.Style.STROKE);
		this.detectionColor.setColor(Color.GREEN);
		this.detectionColor.setStrokeWidth(5);

		this.X_RATIO = frameWidth/(float)width;
		this.Y_RATIO = frameHeight/(float)height;

		this.grayFrame = new Mat(height, width, CvType.CV_8UC1);

		if (mAbsoluteFaceSize == 0) {
			int h = grayFrame.rows();
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

			detector = new CascadeClassifier(mCascadeFile.getAbsolutePath());
			if (detector.empty()) {
				Log.e(TAG, "Failed to load cascade classifier");
				detector = null;
			} else
				Log.i(TAG, "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());

			cascadeDir.delete();

		} catch (IOException e) {
			e.printStackTrace();
			Log.e(">>>", "Failed to load cascade. Exception thrown: " + e);
		}
	}

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
	
	public void setGrid(Grid grid) {
		this.activeGrid = grid;
	}
	
	@Override
	public void process(byte[] data) {
		frame = data;
		grayFrame.put(0, 0, Arrays.copyOfRange(data, 0, width*height));

		MatOfRect faces = new MatOfRect();
		if (detector != null) {
			detector.detectMultiScale(grayFrame, faces, 1.1, 2, 2, new Size(mAbsoluteFaceSize, mAbsoluteFaceSize), new Size());
			facesArray = faces.toArray();
			float[] facesCenter = new float[facesArray.length * 2];
			for(int i = 0, j = 0; i < facesArray.length; i++) {
				facesCenter[j++] = (float) (facesArray[i].tl().x + facesArray[i].width/2) * X_RATIO;
				facesCenter[j++] = (float) (facesArray[i].tl().y + facesArray[i].height/2) * Y_RATIO;
			}
			if(activeGrid != null) {
				if(facesArray.length > 0 && facesArray.length <= 3) {
					float[] nearest = ImageProcessing.nearestPoint(activeGrid.getPowerPoints(), facesCenter);
					activeGrid.highligthPowerPoint(nearest);
				} else {
					activeGrid.highligthPowerPoint(null);
				}
			}
			this.invalidate();
		}
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.FACE_DETECTION);
				} else {
					parent.addOverlay(OverlayType.FACE_DETECTION);
				}
				inflated = !inflated;
			}
		};
	}
}