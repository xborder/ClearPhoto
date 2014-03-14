package tese.helder.clearphoto.overlays.imageprocessing;

import org.opencv.core.Mat;

import tese.helder.clearphoto.overlays.Overlay;
import tese.helder.clearphoto.overlays.grids.Grid;

import android.content.Context;
import android.graphics.Canvas;

public class FaceDetection extends ImageProcessingOv {

	private Grid gridToReplace;
	public Mat frameToProcess;
	
	private FaceDetection(Context context) {
		super(context);
	}

	public FaceDetection(Context context, Grid grid) {
		this(context);
		this.gridToReplace = grid;
	}
	@Override
	protected void onDraw(Canvas canvas) {
		//TODO processar Frame, detectar caras e mandar posições para JNI
	}
}
