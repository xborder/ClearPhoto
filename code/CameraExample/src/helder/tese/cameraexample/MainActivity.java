package helder.tese.cameraexample;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.util.Log;
import android.view.Menu;
import android.widget.FrameLayout;

public class MainActivity extends Activity {

	private final String LOG_TAG = "MainActivity";
	
    private Camera mCamera;
    private CameraPreview mPreview;
    FaceDetection fdetection;
    
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		if(checkCameraHardware(getApplicationContext())) {
			//Load FaceDetection
	        fdetection = new FaceDetection(loadCascadeClassifier());
//	        fdetection.start();
	        
	        // Create an instance of Camera
	        mCamera = getCameraInstance();

	        // Create our Preview view and set it as the content of our activity.
	        mPreview = new CameraPreview(this, mCamera, fdetection);
	        FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
	        preview.addView(mPreview);
	        
		}
	}
	
	private String loadCascadeClassifier() {
		try {
            // load cascade file from application resources
            InputStream is = getResources().openRawResource(R.raw.frontalface);
            File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
            File mCascadeFile = new File(cascadeDir, "frontalface.xml");
            FileOutputStream os = new FileOutputStream(mCascadeFile);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();

            cascadeDir.delete();
            
            return mCascadeFile.getAbsolutePath();
        } catch (IOException e) {
            e.printStackTrace();
            Log.e(LOG_TAG, "Failed to load cascade. Exception thrown: " + e);
        }
		return null;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	protected void onPause() {
		mCamera.stopPreview();
//		fdetection.stop();
		super.onPause();
	}
	
	@Override
	protected void onStop() {
		mCamera.stopPreview();
//		fdetection.stop();
		super.onStop();
	}
	
	@Override
	protected void onDestroy() {
		mCamera.stopPreview();
		super.onDestroy();
	}
	/** Check if this device has a camera */
	private boolean checkCameraHardware(Context context) {
	    if (context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)){
	        // this device has a camera
	        return true;
	    } else {
	        // no camera on this device
	        return false;
	    }
	}

	/** A safe way to get an instance of the Camera object. */
	private Camera getCameraInstance(){
	    Camera c = null;
	    try {
	        c = Camera.open(); // attempt to get a Camera instance
	    }
	    catch (Exception e){
	        // Camera is not available (in use or does not exist)
	    	Log.e(LOG_TAG, "Camera not available");
	    }
	    return c; // returns null if camera is unavailable
	}
	
	/*NDK*/
    
	static {
		System.loadLibrary("facedetection");
    }
}
