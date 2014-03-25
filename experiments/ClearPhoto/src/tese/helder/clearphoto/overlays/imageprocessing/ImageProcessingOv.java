package tese.helder.clearphoto.overlays.imageprocessing;

import android.content.Context;
import android.graphics.Canvas;
import tese.helder.clearphoto.overlays.Overlay;

public abstract class ImageProcessingOv extends Overlay {

	public ImageProcessingOv(Context context, int width, int height) {
		super(context, width, height);
	}

	@Override
	protected abstract void onDraw(Canvas canvas);
	
	public abstract void process(byte[] data);
}
