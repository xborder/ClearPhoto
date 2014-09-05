package tese.helder.clearphoto.overlays.grids;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RadialGradient;
import android.graphics.Shader;
import android.graphics.PorterDuff.Mode;

public class ThirdsGrid extends Grid {
	
	public ThirdsGrid(Context context, int width, int height) {
		super(context, width, height);
		powerPoints = new float[8];
		points = new float[16];
		//First horizontal
		points[0] = 0;			//x0
		points[1] = height/3;	//y0
		points[2] = width;		//x1
		points[3] = height/3;	//y1
		//Second horizontal
		points[4] = 0;			//x0
		points[5] = height*2/3;	//y0
		points[6] = width;		//x1
		points[7] = height*2/3;	//y1
		//First vertical
		points[8] = width/3;	//x0
		points[9] = 0;			//y0
		points[10] = width/3;	//x1
		points[11] = height;	//y1
		//Second vertical
		points[12] = width*2/3;	//x0
		points[13] = 0;			//y0
		points[14] = width*2/3;	//x1
		points[15] = height;	//y1
		
		powerPoints[0] = points[8];
		powerPoints[1] = points[1];
		
		powerPoints[2] = points[12];
		powerPoints[3] = points[1];
		
		powerPoints[4] = points[8];
		powerPoints[5] = points[5];
		
		powerPoints[6] = points[12];
		powerPoints[7] = points[5];
	}

	@Override
	protected void onDraw(Canvas canvas) {
		if(!toDraw) {
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			return;
		}
		canvas.drawLines(points, gridColor);

		drawSuggestivePowerPoints(canvas);
		drawSuggestiveComposition(canvas);
	}
	@Override
	protected void onAttachedToWindow() {
		toDraw = true;
		super.onAttachedToWindow();
	}
	
	@Override
	protected void onDetachedFromWindow() {
		toDraw = false;
		invalidate();
		super.onDetachedFromWindow();
	}
}
