package tese.helder.clearphoto.overlays.grids;

import android.content.Context;
import android.graphics.Canvas;

public class TriangleGrid extends Grid {

	public TriangleGrid(Context context, int width, int height) {
		super(context, width, height);
	}

	@Override
	protected void onDraw(Canvas canvas) {		
		canvas.drawLine(0, height, width, 0, gridColor); 				//Main line
		canvas.drawLines(lineIntersections(0, 0), gridColor); 			//First Intersecting Line
		canvas.drawLines(lineIntersections(width, height), gridColor); 	//Second Intersecting Line
	}

	private float[] lineIntersections(float x0, float y0) {
		float m1 = (0-(float)height)/((float)width-0);
		float m2 = -1/m1;
		
		float px = (-m2*x0+y0-height)/(m1-m2);
		float py = m1 * px + height;
		
		return new float[]{x0,y0,px,py};
	}
}
