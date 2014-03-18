package tese.helder.clearphoto;

import org.opencv.android.OpenCVLoader;

import android.hardware.Camera;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
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

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);		
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.activity_main);

		cameraViewer = new CameraViewer(this, getCameraInstance());
		FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
		preview.addView(cameraViewer);

		registerEvents();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	protected void onResume() {
		super.onResume();
	}

	@Override
	protected void onPause() {
		super.onPause();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	/** A safe way to get an instance of the Camera object. */
	public static Camera getCameraInstance(){
		Camera c = null;
		try {
			c = Camera.open(); // attempt to get a Camera instance
		}
		catch (Exception e){
			// Camera is not available (in use or does not exist)
		}
		return c; // returns null if camera is unavailable
	}

	private void registerEvents() {
		cameraViewer.setOnLongClickListener(new OnLongClickListener() {
			@Override
			public boolean onLongClick(View v) {
				GridSelectionControl gridControl = new GridSelectionControl(MainActivity.this, cameraViewer);
				gridControl.showDialog();
				return true;
			}
		});
	}
}
