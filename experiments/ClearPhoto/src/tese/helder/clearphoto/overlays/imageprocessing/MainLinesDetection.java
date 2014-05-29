package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
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
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.HapticFeedbackConstants;
import android.view.MotionEvent;
import android.view.View;

//TODO melhorar a mudança de threshold (add animaçoes, aumentar consoante a rapidez do swipe)
public class MainLinesDetection extends ImageProcessingOv {
	private float X_RATIO;
	private float Y_RATIO;
	private Mat data;
	private ArrayList<Integer> linesToDraw;
	private long nativeAddr; 
	private int threshold;
	private Paint linesPaint;
	private GestureDetector gDetector;

	public MainLinesDetection(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);

		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);

		this.X_RATIO = width/(float)previewWidth;
		this.Y_RATIO = height/(float)previewHeight;

		this.linesToDraw = new ArrayList<Integer>();
		this.threshold = previewWidth > previewHeight ? previewWidth/4 : previewHeight/4;;

		this.linesPaint = new Paint();
		this.linesPaint.setStyle(Paint.Style.STROKE);
		this.linesPaint.setColor(Color.BLUE);
		this.linesPaint.setStrokeWidth(2);

		this.setOnTouchListener(new OnSwipeTouchListener(context));
	}

	@Override
	protected void onAttachedToWindow() {
		super.onAttachedToWindow();
		nativeAddr = ImageProcessing.initMainLinesDetection();
	}

	@Override
	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();
		ImageProcessing.stopMainLinesDetection(nativeAddr);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		for(int offset = 0; offset < linesToDraw.size(); offset+=4) {
			canvas.drawLine(linesToDraw.get(offset+0)*X_RATIO, linesToDraw.get(offset+1)*Y_RATIO, 
					linesToDraw.get(offset+2)*X_RATIO, linesToDraw.get(offset+3)*Y_RATIO, linesPaint);
		}
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		linesToDraw.clear();
		ImageProcessing.getMainLines(nativeAddr, data, threshold, linesToDraw);
		this.invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.MAIN_LINES);
				} else {
					parent.addOverlay(OverlayType.MAIN_LINES);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}

	class OnSwipeTouchListener implements OnTouchListener {

		private final GestureDetector gestureDetector;

		public OnSwipeTouchListener (Context ctx){
			gestureDetector = new GestureDetector(ctx, new GestureListener());
		}

		private final class GestureListener extends SimpleOnGestureListener {

			private static final int SWIPE_THRESHOLD = 100;
			private static final int SWIPE_VELOCITY_THRESHOLD = 100;

			@Override
			public boolean onDown(MotionEvent e) {
				return true;
			}

			@Override
			public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
				boolean result = false;
				try {
					float diffY = e2.getY() - e1.getY();
					float diffX = e2.getX() - e1.getX();

					if (Math.abs(diffY) > SWIPE_THRESHOLD && Math.abs(velocityY) > SWIPE_VELOCITY_THRESHOLD) {
						if (diffY > 0) {
							onSwipeBottom();
						} else {
							onSwipeTop();
						}
					}
				} catch (Exception exception) {
					exception.printStackTrace();
				}
				return result;
			}
		}
		public void onSwipeTop() {
			threshold += 5;
		}
		public void onSwipeBottom() {
			threshold -= 5;
		}

		public boolean onTouch(View v, MotionEvent event) {
			return gestureDetector.onTouchEvent(event);
		}
	}

}
