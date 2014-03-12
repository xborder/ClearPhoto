package tese.helder.clearphoto;


import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;

/** A basic Camera preview class */
public class CameraViewer extends SurfaceView implements SurfaceHolder.Callback, PreviewCallback {
    private final static String TAG = "CameraViewer";
	
	private SurfaceHolder mHolder;
    private Camera mCamera;
    private ImageView img;
    private int width, height;

    public CameraViewer(Context context, Camera camera) {
        super(context);
        mCamera = camera;
        
        Camera.Parameters mParams = mCamera.getParameters();
        List<Camera.Size> previewSizes = mParams.getSupportedPreviewSizes();
        Camera.Size previewSize = previewSizes.get(previewSizes.size()-1);
        
        width = previewSize.width;
        height = previewSize.height;
        // Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed.
        mHolder = getHolder();
        mHolder.addCallback(this);

    }
    public CameraViewer(ImageView im, Context c, Camera camera) {
    	this(c,camera);
    	img = im;
    }
    
    public void surfaceCreated(SurfaceHolder holder) {
        // The Surface has been created, now tell the camera where to draw the preview.
        try {
            mCamera.setPreviewDisplay(holder);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        } catch (IOException e) {
            Log.d(TAG, "Error setting camera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        // empty. Take care of releasing the Camera preview in your activity.
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        // If your preview can change or rotate, take care of those events here.
        // Make sure to stop the preview before resizing or reformatting it.

        if (mHolder.getSurface() == null){
          // preview surface does not exist
          return;
        }

        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch (Exception e){
          // ignore: tried to stop a non-existent preview
        }

        // set preview size and make any resize, rotate or
        // reformatting changes here

        // start preview with new settings
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();

        } catch (Exception e){
            Log.d(TAG, "Error starting camera preview: " + e.getMessage());
        }
    }

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {

	    Log.w(TAG, "hero");
		Mat mYuv = new Mat( height + height/2, width, CvType.CV_8UC1 );
	    mYuv.put( 0, 0, data );
	    
	    Mat mRgba = new Mat();
	    Imgproc.cvtColor( mYuv, mRgba, Imgproc.COLOR_YUV2RGBA_NV21, 4 );
	    Imgproc.resize(mRgba, mRgba, new org.opencv.core.Size(600, 480));

//	    ImageView view = (ImageView) findViewById(R.id.effects);
	    
	    byte[] image = new byte[(int)mRgba.total() * mRgba.channels()];
	    mRgba.get(0, 0, image);

	    img.setImageDrawable(null);
	    img.setImageBitmap(BitmapFactory.decodeByteArray(image, 0, image.length));
	}
}