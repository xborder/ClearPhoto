package tese.helder.clearphoto;

import tese.helder.clearphoto.overlays.grids.Grid;
import tese.helder.clearphoto.overlays.imageprocessing.ColorHistogram;
import tese.helder.clearphoto.overlays.imageprocessing.ColorWheel;
import tese.helder.clearphoto.overlays.imageprocessing.FaceDetection;
import tese.helder.clearphoto.overlays.imageprocessing.HorizonDetection;
import tese.helder.clearphoto.overlays.imageprocessing.MainLinesDetection;
import tese.helder.clearphoto.overlays.imageprocessing.SaturationDetection;
import android.app.Activity;
import android.graphics.PixelFormat;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.ImageButton;

public class OptionsView {

	private LayoutInflater inflater;
	private Activity act;
	private CameraViewer cameraViewer;
	private View view;
	private boolean inflated;

	public OptionsView(Activity act, CameraViewer viewer) {
		this.act = act;
		this.cameraViewer = viewer;
		this.inflater = LayoutInflater.from(act);
		this.view = inflater.inflate(R.layout.options, null);

		registerEvents();
	}

	public void inflate() {
		if(!inflated) {
			act.addContentView(view, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
			inflated = true;
		}
	}

	public void deflate() {
		if(inflated) {
			ViewGroup vg = (ViewGroup)(view.getParent());
			vg.removeView(view);
			inflated = false;
		}
	}

	private void registerEvents() {
		ImageButton button = null;
		button = (ImageButton)view.findViewById(R.id.gridButton);
		button.setOnClickListener(Grid.getOnClickListener(act, cameraViewer));
 
		button = (ImageButton)view.findViewById(R.id.faceDetectionButton);
		button.setOnClickListener(FaceDetection.getOnClickListener(cameraViewer));

		button = (ImageButton)view.findViewById(R.id.histogramButton);
		button.setOnClickListener(ColorHistogram.getOnClickListener(cameraViewer));

		button = (ImageButton)view.findViewById(R.id.colorWheelButton);
		button.setOnClickListener(ColorWheel.getOnClickListener(cameraViewer));

		button = (ImageButton)view.findViewById(R.id.saturationDetectionButton);
		button.setOnClickListener(SaturationDetection.getOnClickListener(cameraViewer));

		button = (ImageButton)view.findViewById(R.id.horizonDetectionButton);
		button.setOnClickListener(HorizonDetection.getOnClickListener(cameraViewer));
		
		button = (ImageButton)view.findViewById(R.id.majorLinesButton);
		button.setOnClickListener(MainLinesDetection.getOnClickListener(cameraViewer));
	}
}
