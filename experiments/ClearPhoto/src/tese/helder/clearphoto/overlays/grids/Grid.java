package tese.helder.clearphoto.overlays.grids;

import tese.helder.clearphoto.overlays.Overlay;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

public abstract class Grid extends Overlay {

	protected Paint gridColor;
	
	public Grid(Context context, int width, int height) {
		super(context, width, height);
		this.gridColor = new Paint();
		this.gridColor.setStyle(Paint.Style.FILL);
		this.gridColor.setColor(Color.WHITE);
		this.gridColor.setStrokeWidth(5);
	}

	@Override
	protected abstract void onDraw(Canvas canvas);
}
