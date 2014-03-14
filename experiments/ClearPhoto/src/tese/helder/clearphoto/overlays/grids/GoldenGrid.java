package tese.helder.clearphoto.overlays.grids;

import android.content.Context;
import android.graphics.Canvas;
import android.util.Log;

public class GoldenGrid extends Grid{

	private final static float RATIO = (float) 1.6180339887498948482045868343656381177203091798057628;
	
	public GoldenGrid(Context context, int width, int height) {
		super(context, width, height);
	}

	@Override
	protected void onDraw(Canvas canvas) {

		Log.w("asdsad", height*RATIO+" "+ width +" "+ height*RATIO);
		canvas.drawLine(0, height/RATIO, width, height/RATIO, gridColor); //First horizontal
		canvas.drawLine(0, height-height/RATIO, width, height-height/RATIO, gridColor); //Second horizontal

		canvas.drawLine(width/RATIO, 0, width/RATIO, height, gridColor); //First vertical
		canvas.drawLine(width-width/RATIO, 0, width-width/RATIO, height, gridColor); //Second vertical
	}

}
