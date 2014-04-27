package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.PriorityQueue;
import java.util.Random;
import java.util.SortedSet;
import java.util.TreeSet;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import tese.helder.clearphoto.ImageProcessing;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Canvas.VertexMode;
import android.graphics.ColorFilter;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.shapes.Shape;
import android.graphics.Color;
import android.graphics.Paint;
import android.support.v4.widget.DrawerLayout.DrawerListener;
import android.util.Log;

public class ColorWheel extends ImageProcessingOv
{
	private static int TRIANGLE_LATERAL_SIZE = 100;
	private static int BORDER = 30;
	private static int WHEEL_COLORS = 6;
	private static int ROTATION_INCREMENT = 60;
	private static int[] COLORS = {Color.RED, Color.YELLOW, Color.GREEN, Color.CYAN, Color.BLUE, Color.MAGENTA};
	private static int[] HUE_RANGES = {0,30,60,90,120,150,180};

	private Mat data;
	private int[] colorClusters;
	private List<WheelTriangle> triangles;
	private TreeSet<WheelTriangle> toShow;
	private int triangle_h;
	private int previewHeight, previewWidth;
	private int maxColorAmount;
	
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

		triangle_h = (int)Math.sqrt(Math.pow(TRIANGLE_LATERAL_SIZE,2) - Math.pow(TRIANGLE_LATERAL_SIZE/2, 2));
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		int center_x = width - (triangle_h + BORDER);
		int center_y = triangle_h + BORDER;
		int angle = 0;
		for(int i = 0; i < WHEEL_COLORS; i ++) { // Triangles creation
			triangles.add(new WheelTriangle(center_x, center_y, COLORS[i], angle));
			angle += ROTATION_INCREMENT;
		}
//		for(int i = 0; i < WHEEL_COLORS; i ++) { // Setting complementary triangles for each one
//			WheelTriangle cur = triangles.get(i);
//			cur.setComplementaryTriangle(triangles.get((i + 3) % WHEEL_COLORS));
//		}
	}

	@Override
	protected void onDraw(Canvas canvas) {
		for(WheelTriangle wt: triangles) {
//			invalidateDrawable(wt);
			wt.draw(canvas);
			wt.invalidateSelf();
		}
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		ImageProcessing.getHueConcentrarion(data, colorClusters, HUE_RANGES, previewWidth, previewHeight);
		toShow.clear();
		for(int i = 0; i < WHEEL_COLORS; i++) {
			WheelTriangle t = triangles.get(i);
			t.setAmount(colorClusters[i]/(float)maxColorAmount);
			toShow.add(t);
		}

		Log.e(">>>>>>", Arrays.toString(colorClusters));
		Log.e("----", toShow.toString());
		for(int i = 0; i < WHEEL_COLORS; i++) {
			WheelTriangle t = toShow.pollFirst();
			if(t != null)
				t.showAmount(true);
		}
		
		this.invalidate();
	}
	
	private class WheelTriangle extends Drawable {
		private Point center, vertice1, vertice2;
		private Paint color, inactiveColor, clearColor;
		private Path mainShape, amountShape;
//		private WheelTriangle complementary;
		private boolean highlight, showAmount;
		private float scale;
		private float angle;
		
		public WheelTriangle(int center_x, int center_y, int colorCode, int angle) {
			this.center = new Point(center_x, center_y);
			this.vertice1 = new Point(center.x - TRIANGLE_LATERAL_SIZE / 2, center.y - triangle_h);
			this.vertice2 = new Point(center.x + TRIANGLE_LATERAL_SIZE / 2, center.y - triangle_h);
			this.angle = angle;
			
			this.highlight = false;
			this.amountShape = new Path();
			this.mainShape = new Path();
			
			mainShape.moveTo(center.x, center.y);
			mainShape.lineTo(vertice1.x, vertice1.y);
			mainShape.lineTo(vertice2.x, vertice2.y);
			mainShape.lineTo(center.x, center.y);
			
			inactiveColor = new Paint();
			inactiveColor.setStyle(Paint.Style.FILL_AND_STROKE);
			inactiveColor.setColor(Color.parseColor("#D9444444"));
			
			color = new Paint();
			color.setStyle(Paint.Style.FILL_AND_STROKE);
			color.setColor(colorCode);
			
			clearColor = new Paint();
			clearColor.setStyle(Paint.Style.FILL_AND_STROKE);
			clearColor.setColor(Color.TRANSPARENT);
		}
		
		@Override
		public void draw(Canvas canvas) {
			int savedState = canvas.save();
			canvas.rotate(angle, center.x, center.y);
			
			canvas.drawPath(mainShape, clearColor);
			canvas.drawPath(mainShape, color);
			if(!highlight) {
				canvas.drawPath(mainShape, inactiveColor);
			}
			Random r = new Random();
			if(showAmount) {
				canvas.drawPath(amountShape, color);
//				complementary.highlightTriangle(true);
				showAmount(false);
//			} else {
//				complementary.highlightTriangle(false);
			}
			canvas.restoreToCount(savedState);
		}

		@Override
		public int getOpacity() { return 0; }

		@Override
		public void setAlpha(int alpha) {}

		@Override
		public void setColorFilter(ColorFilter cf) {}
		
//		public void setComplementaryTriangle(WheelTriangle complementary) {
//			this.complementary = complementary;
//		}
		
		public void showAmount(boolean value) {
			this.showAmount = value;
//			this.complementary.highlightTriangle(value);
		}
		
		public void setAmount(float scale) {
			if(!showAmount) return;
			
			this.scale = scale;
			int new_triangle_h = (int)Math.sqrt(Math.pow(TRIANGLE_LATERAL_SIZE*scale,2) - Math.pow(TRIANGLE_LATERAL_SIZE*scale/2, 2));
			int v1_x = (int)(center.x - TRIANGLE_LATERAL_SIZE*scale / 2), v1_y = (int)(center.y - new_triangle_h);
			int v2_x = (int)(center.x + TRIANGLE_LATERAL_SIZE*scale / 2), v2_y = (int)(center.y - new_triangle_h);
			
			amountShape.reset();
			amountShape.moveTo(center.x, center.y);
			amountShape.lineTo(v1_x, v1_y);
			amountShape.lineTo(v2_x, v2_y);
			amountShape.lineTo(center.x, center.y);
		}
		
		public void highlightTriangle(boolean highlight) {
			this.highlight = highlight;
		}

		@Override
		public String toString() {
			return this.color.getColor() + " / "+ this.scale;
		}
		
		
	} 

}
