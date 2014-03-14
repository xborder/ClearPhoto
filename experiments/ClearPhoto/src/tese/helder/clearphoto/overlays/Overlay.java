package tese.helder.clearphoto.overlays;

import android.content.Context;
import android.graphics.Canvas;
import android.view.View;

public abstract class Overlay extends View {

	public Overlay(Context context) {
		super(context);
	}

	@Override
	protected abstract void onDraw(Canvas canvas);
	
}
