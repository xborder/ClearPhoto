package tese.helder.clearphoto;


import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import tese.helder.clearphoto.overlays.Overlay;
import tese.helder.clearphoto.overlays.OverlayType;
import tese.helder.clearphoto.overlays.grids.GoldenGrid;
import tese.helder.clearphoto.overlays.grids.Grid;
import tese.helder.clearphoto.overlays.grids.ThirdsGrid;
import tese.helder.clearphoto.overlays.grids.TriangleGrid;
import tese.helder.clearphoto.overlays.imageprocessing.ImageProcessingOv;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.util.Log;
import android.util.Pair;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;

/** A basic Camera preview class */
public class CameraViewer extends SurfaceView implements SurfaceHolder.Callback, PreviewCallback {
    private final static String TAG = "CameraViewer";
	
	private SurfaceHolder mHolder;
    private Camera mCamera;

    private Grid activeGrid;
	private List<Pair<ImageProcessingOv, LayoutParams>> imageProcessingOv;
	
	private Activity act;
	
	public CameraViewer(Activity act, Camera camera) {
		this(act.getBaseContext(), camera);
		this.act = act;
	}
	
    public CameraViewer(Context context, Camera camera) {
        super(context);

		imageProcessingOv = new ArrayList<Pair<ImageProcessingOv, LayoutParams>>();
		
        mCamera = camera;
        
//        Camera.Parameters mParams = mCamera.getParameters();
//        List<Camera.Size> previewSizes = mParams.getSupportedPreviewSizes();
//        Camera.Size previewSize = previewSizes.get(previewSizes.size()-1);
//        
//        width = previewSize.width;
//        height = previewSize.height;
        // Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed.
        mHolder = getHolder();
        mHolder.addCallback(this);
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
    	mCamera.setPreviewCallback(null);
    	mCamera.stopPreview();
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
        	mCamera.setPreviewCallback(null);
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
    protected void onDraw(Canvas canvas) {
    	super.onDraw(canvas);
    }
    
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		
	}
	
	public void addOverlay(OverlayType ov) {
		if(activeGrid != null) {
			ViewGroup vg = (ViewGroup)(activeGrid.getParent());
			vg.removeView(activeGrid);
		}
		
		if(ov == OverlayType.GRID_THIRDS) {
			activeGrid = new ThirdsGrid(getContext(), getWidth(), getHeight());
		} else if (ov == OverlayType.GRID_THIRDS_GOLDEN) {
			activeGrid = new GoldenGrid(getContext(), getWidth(), getHeight());
		} else if (ov == OverlayType.GRID_GOLDEN_TRIANGLES) {
			activeGrid = new TriangleGrid(getContext(), getWidth(), getHeight());
		}
		act.addContentView(activeGrid, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
	}
	

}
