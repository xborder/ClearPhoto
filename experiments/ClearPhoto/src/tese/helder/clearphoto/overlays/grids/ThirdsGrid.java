package tese.helder.clearphoto.overlays.grids;

import android.content.Context;
import android.graphics.Canvas;

public class ThirdsGrid extends Grid {
	
	public ThirdsGrid(Context context, int width, int height) {
		super(context, width, height);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		canvas.drawLine(0, height/3, width, height/3, gridColor); //First horizontal
		canvas.drawLine(0, height*2/3, width, height*2/3, gridColor); //Second horizontal

		canvas.drawLine(width/3, 0, width/3, height, gridColor); //First vertical
		canvas.drawLine(width*2/3, 0, width*2/3, height, gridColor); //Second vertical
	}
}
