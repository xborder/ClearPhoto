package tese.helder.clearphoto.overlays.imageprocessing;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

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
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.View;

public class ColorTemplates extends ImageProcessingOv{
	private static int MARGIN = 20;
	private static int WHEEL_COLORS = 12;
	private static int ROTATION_INCREMENT = 30;
	
	private Mat data;
	private int previewWidth, previewHeight;
	private Bitmap wheel;
	private int wheel_width, wheel_height;
	private Point wheelCenter;
	private List<Triangle> triangles;
	private List<Integer> result;
	
	public ColorTemplates(Context context, int previewWidth, int previewHeight, int width, int height) {
		super(context, width, height);
		this.data = new Mat(previewHeight + previewHeight/2, previewWidth, CvType.CV_8UC1);
		this.previewHeight = previewHeight;
		this.previewWidth = previewWidth;
		this.result = new ArrayList<Integer>();
		
		BitmapFactory.Options dimensions = new BitmapFactory.Options(); 
		dimensions.inJustDecodeBounds = true;
		BitmapFactory.decodeResource(getResources(), R.drawable.color_template, dimensions);
		
		this.wheel = BitmapFactory.decodeResource(getResources(), R.drawable.color_template);
		this.wheel_height = dimensions.outHeight;
		this.wheel_width =  dimensions.outWidth;
		this.wheelCenter = new Point(wheel_width/2 + MARGIN, height - wheel_height/2 - MARGIN);
		
		this.triangles = new ArrayList<Triangle>();
		int angle = 0;
		for(int i = 0; i < WHEEL_COLORS; i ++) { // Triangles creation
			this.triangles.add(new Triangle(angle));
			angle += ROTATION_INCREMENT;
		}
	}

	@Override
	protected void onDraw(Canvas canvas) {
		for(Integer i: result) {
			triangles.get(i).draw(canvas);
		}
		canvas.drawBitmap(wheel, MARGIN, height-wheel_height-MARGIN, null);
	}

	@Override
	public void process(byte[] data_) {
		data.put(0, 0, Arrays.copyOfRange(data_, 0, data_.length));
		result = ImageProcessing.getColorTemplate(data);
		Log.w("asds", result.toString());
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
	

	private class Triangle extends Drawable {
		private Point vertice1, vertice2;
		private Paint color;
		private Path mainShape;
		private float angle;

		public Triangle(int angle) {
			this.vertice1 = new Point(wheelCenter.x - 24, wheelCenter.y - 80);
			this.vertice2 = new Point(wheelCenter.x + 24, wheelCenter.y - 80);
			this.angle = angle;
			
			this.mainShape = new Path();

			this.mainShape.moveTo(wheelCenter.x, wheelCenter.y);
			this.mainShape.lineTo(vertice1.x, vertice1.y);
			this.mainShape.lineTo(vertice2.x, vertice2.y);
			this.mainShape.lineTo(wheelCenter.x, wheelCenter.y);
			
			this.color = new Paint();
			this.color.setStyle(Paint.Style.FILL_AND_STROKE);
			this.color.setColor(Color.GRAY);
		}

		@Override
		public void draw(Canvas canvas) {
			int savedState = canvas.save();
			canvas.rotate(angle, wheelCenter.x, wheelCenter.y);
			canvas.drawPath(mainShape, color);
			canvas.restoreToCount(savedState);
		}

		@Override
		public int getOpacity() { return 0; }

		@Override
		public void setAlpha(int alpha) {}

		@Override
		public void setColorFilter(ColorFilter cf) {}
	} 

}
