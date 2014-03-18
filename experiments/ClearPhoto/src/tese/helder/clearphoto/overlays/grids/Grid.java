package tese.helder.clearphoto.overlays.grids;

import tese.helder.clearphoto.overlays.Overlay;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ComposeShader;
import android.graphics.Paint;
import android.graphics.RadialGradient;
import android.graphics.Shader;

public abstract class Grid extends Overlay {

	protected Paint gridColor;
	protected Paint highlightColor;
	protected float[] powerPoints;
	protected float[] points;
	protected boolean highlightPoints;

	public Grid(Context context, int width, int height) {
		super(context, width, height);
		this.gridColor = new Paint();
		this.gridColor.setStyle(Paint.Style.FILL);
		this.gridColor.setColor(Color.WHITE);
		this.gridColor.setStrokeWidth(2.5f);


		this.highlightColor = new Paint();
	}

	@Override
	protected abstract void onDraw(Canvas canvas);

	public float[] getPowerPoints() {
		return this.powerPoints;
	}

	public void highligthPowerPoint(float[] nearest) {
		if(nearest == null) {
			this.highlightPoints = false;
		} else {
			this.highlightPoints = true;
			this.highlightColor.setShader(new RadialGradient(nearest[0], nearest[1], 25, Color.RED, Color.TRANSPARENT, Shader.TileMode.CLAMP));
		}
		this.invalidate();
	}

}
