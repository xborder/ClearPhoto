package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.TreeSet;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.ImageProcessing;
import tese.helder.clearphoto.overlays.OverlayType;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.HapticFeedbackConstants;
import android.view.View;

public class ColorWheel extends ImageProcessingOv
{
	private static int[] TRIANGLE_LATERAL_SIZE = {100, 85};
	private static int BORDER = 30;
	private static int WHEEL_COLORS = 6;
	private static int ROTATION_INCREMENT = 60;
	private static int WHEEL_MAX_H = 100, WHEEL_MAX_W = 100;
	private static int[] COLORS = {Color.RED, Color.YELLOW, Color.GREEN, Color.CYAN, Color.BLUE, Color.MAGENTA};
	private static int[] HUE_RANGES = {15, 45, 75, 105, 135, 165};//{0,30,60,90,120,150,180};

	private Mat data;
	private int[] colorClusters;
	private List<WheelTriangle> triangles;
	private TreeSet<WheelTriangle> toShow;
	private int previewHeight, previewWidth;
	private int maxColorAmount;
	private Point wheelCenter;

	public ColorWheel(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		maxColorAmount = previewWidth*previewHeight;
		colorClusters = new int[WHEEL_COLORS];
		triangles = new ArrayList<WheelTriangle>();
		toShow = new TreeSet<WheelTriangle>(new Comparator<WheelTriangle>() {
			@Override
			public int compare(WheelTriangle lhs, WheelTriangle rhs) {
				if(lhs.scale > rhs.scale)
					return -1;
				else if (lhs.scale < rhs.scale)
					return 1;
				return 0;
			}
		});

		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.wheelCenter = new Point(width - (WHEEL_MAX_W + BORDER), WHEEL_MAX_H + BORDER);
		int angle = 0;
		for(int i = 0; i < WHEEL_COLORS; i ++) { // Triangles creation
			triangles.add(new WheelTriangle(COLORS[i], angle, TRIANGLE_LATERAL_SIZE[i % TRIANGLE_LATERAL_SIZE.length]));
			angle += ROTATION_INCREMENT;
		}
	}

	@Override
	protected void onDraw(Canvas canvas) {
		for(WheelTriangle wt: triangles) {
			wt.draw(canvas);
		}
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.getHueConcentrarion(data, colorClusters, HUE_RANGES, previewWidth, previewHeight);
		toShow.clear();
		for(int i = 0; i < WHEEL_COLORS; i++) {
			WheelTriangle t = triangles.get((i+1)%WHEEL_COLORS);
			t.setAmount(colorClusters[i]/(float)maxColorAmount);
			toShow.add(t);
		}

		for(int i = 0; i < WHEEL_COLORS; i++) {
			WheelTriangle t = toShow.pollFirst();
			if(t != null)
				t.showAmount(true);
		}

		this.invalidate();
	}

	public static OnClickListener getOnClickListener(final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;

			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.COLOR_WHEEL);
				} else {
					parent.addOverlay(OverlayType.COLOR_WHEEL);
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}

	private class WheelTriangle extends Drawable {
		private static final String INACTIVE_COLOR_CODE = "#D9444444";
		
		private Point vertice1, vertice2;
		private Paint color, inactiveColor;
		private Path mainShape, amountShape;
		private boolean showAmount;
		private float scale;
		private float angle;
		private int lateralSize, height;

		public WheelTriangle(int colorCode, int angle, int lateralSize) {
			this.lateralSize = lateralSize;
			this.height = (int) Math.sqrt(Math.pow(lateralSize,2) - Math.pow(lateralSize/2, 2));
			this.vertice1 = new Point(wheelCenter.x - lateralSize / 2, wheelCenter.y - height);
			this.vertice2 = new Point(wheelCenter.x + lateralSize / 2, wheelCenter.y - height);
			this.angle = angle;

			this.amountShape = new Path();
			this.mainShape = new Path();

			mainShape.moveTo(wheelCenter.x, wheelCenter.y);
			mainShape.lineTo(vertice1.x, vertice1.y);
			mainShape.lineTo(vertice2.x, vertice2.y);
			mainShape.lineTo(wheelCenter.x, wheelCenter.y);

			inactiveColor = new Paint();
			inactiveColor.setStyle(Paint.Style.FILL_AND_STROKE);
			inactiveColor.setColor(Color.parseColor(INACTIVE_COLOR_CODE));

			color = new Paint();
			color.setStyle(Paint.Style.FILL_AND_STROKE);
			color.setColor(colorCode);
		}

		@Override
		public void draw(Canvas canvas) {
			int savedState = canvas.save();
			canvas.rotate(angle, wheelCenter.x, wheelCenter.y);

			canvas.drawPath(mainShape, color);
			canvas.drawPath(mainShape, inactiveColor);
			if(showAmount) {
				canvas.drawPath(amountShape, color);
				showAmount(false);
			}
			canvas.restoreToCount(savedState);
		}

		@Override
		public int getOpacity() { return 0; }

		@Override
		public void setAlpha(int alpha) {}

		@Override
		public void setColorFilter(ColorFilter cf) {}

		public void showAmount(boolean value) {
			this.showAmount = value;
		}

		public void setAmount(float scale) {
			if(!showAmount) return;

			this.scale = scale;
			int new_triangle_h = (int)Math.sqrt(Math.pow(lateralSize*scale,2) - Math.pow(lateralSize*scale/2, 2));
			int v1_x = (int)(wheelCenter.x - lateralSize*scale / 2), v1_y = (int)(wheelCenter.y - new_triangle_h);
			int v2_x = (int)(wheelCenter.x + lateralSize*scale / 2), v2_y = (int)(wheelCenter.y - new_triangle_h);

			amountShape.reset();
			amountShape.moveTo(wheelCenter.x, wheelCenter.y);
			amountShape.lineTo(v1_x, v1_y);
			amountShape.lineTo(v2_x, v2_y);
			amountShape.lineTo(wheelCenter.x, wheelCenter.y);
		}

		@Override
		public String toString() {
			return this.color.getColor() + " / "+ this.scale;
		}


	} 

}
