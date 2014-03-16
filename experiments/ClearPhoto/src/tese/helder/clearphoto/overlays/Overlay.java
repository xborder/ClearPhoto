package tese.helder.clearphoto.overlays;

import android.content.Context;
import android.graphics.Canvas;
import android.view.View;

public abstract class Overlay extends View {
	protected int width, height;
	
	public Overlay(Context context, int width, int height) {
		super(context);
		this.width = width;
		this.height = height;
	}

	@Override
	protected abstract void onDraw(Canvas canvas);
	
}
