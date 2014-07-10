package tese.helder.clearphoto;

import org.opencv.android.OpenCVLoader;

import android.hardware.Camera;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.WindowManager;
import android.widget.FrameLayout;

public class MainActivity extends Activity {

	static {
		if (!OpenCVLoader.initDebug()) {
		} else {
			System.loadLibrary("image_processing");
		}
	}

	private CameraViewer cameraViewer;
	private OptionsView options;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);		
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.activity_main);

		cameraViewer = new CameraViewer(this);
		FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
		preview.addView(cameraViewer);

		options = new OptionsView(this, cameraViewer);
		registerEvents();
	}

	@Override
	protected void onPause() {
		cameraViewer.releaseCamera();
		super.onPause();
	}
	
	@Override
	protected void onResume() {
		cameraViewer.openCamera();
		super.onResume();
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}


	private void registerEvents() {
		cameraViewer.setOnLongClickListener(new OnLongClickListener() {
			@Override
			public boolean onLongClick(View v) {
				options.inflate();
				return true;
			}
		});
		cameraViewer.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				options.deflate();
			}
		});
	}
}
