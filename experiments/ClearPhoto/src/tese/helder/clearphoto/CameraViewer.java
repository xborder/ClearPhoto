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
import tese.helder.clearphoto.overlays.imageprocessing.ColorHistogram;
import tese.helder.clearphoto.overlays.imageprocessing.ColorWheel;
import tese.helder.clearphoto.overlays.imageprocessing.FaceDetection;
import tese.helder.clearphoto.overlays.imageprocessing.HorizonDetection;
import tese.helder.clearphoto.overlays.imageprocessing.ImageProcessingOv;
import tese.helder.clearphoto.overlays.imageprocessing.MainLinesDetection;
import tese.helder.clearphoto.overlays.imageprocessing.ObjectSegmentation;
import tese.helder.clearphoto.overlays.imageprocessing.SaturationDetection;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
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
    private FaceDetection faceDetection;
    private ColorHistogram colorHistogram;
    private ColorWheel colorWheel;
    private SaturationDetection saturation;
    private HorizonDetection horizonDetection;
    private MainLinesDetection majorLines;
    private ObjectSegmentation objectSegmentation;
    
    private int previewWidth, previewHeight;
	private List<Pair<ImageProcessingOv, LayoutParams>> imageProcessingOv;
	
	private Activity act;
	
	public CameraViewer(Activity act, Camera camera) {
		this(act.getBaseContext(), camera);
		this.act = act;
	}
	
    private CameraViewer(Context context, Camera camera) {
        super(context);
        imageProcessingOv = new ArrayList<Pair<ImageProcessingOv, LayoutParams>>();
		
        mCamera = camera;
        
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
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setSceneMode(Camera.Parameters.SCENE_MODE_NIGHT);
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
            
    		previewWidth = mCamera.getParameters().getPreviewSize().width;
    		previewHeight = mCamera.getParameters().getPreviewSize().height;
            
    		mCamera.setParameters(parameters);
            mCamera.setPreviewDisplay(mHolder);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        } catch (Exception e){
            Log.d(TAG, "Error starting camera preview: " + e.getMessage());
        }
    }

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		if(faceDetection != null) {
			faceDetection.process(data);
		}
		if(colorHistogram != null) {
			colorHistogram.process(data);
		}
		if(colorWheel != null) {
			colorWheel.process(data);
		}
		if(saturation != null) {
			saturation.process(data);
		}
		if(horizonDetection != null){
			horizonDetection.process(data);
		}
		if(majorLines != null) {
			majorLines.process(data);
		}
		if(objectSegmentation != null) {
			objectSegmentation.process(data);
		}
	}
	
	public void removeOverlay(OverlayType ov) {
		ViewGroup vg;
		if (ov == OverlayType.GRID) {
			vg = (ViewGroup) activeGrid.getParent();
			vg.removeView(activeGrid);
			activeGrid = null;
			if(faceDetection != null){
				faceDetection.setGrid(null);
			}
		} else if (ov == OverlayType.FACE_DETECTION) {
			vg = (ViewGroup) faceDetection.getParent();
			vg.removeView(faceDetection);
			faceDetection = null;
		} else if (ov == OverlayType.COLOR_HIST) {
			vg = (ViewGroup) colorHistogram.getParent();
			vg.removeView(colorHistogram);
			colorHistogram = null;
		} else if (ov == OverlayType.COLOR_WHEEL) {
			vg = (ViewGroup) colorWheel.getParent();
			vg.removeView(colorWheel);
			colorWheel = null;
		} else if (ov == OverlayType.SATURATION_DETECTION) {
			vg = (ViewGroup) saturation.getParent();
			vg.removeView(saturation);
			saturation = null;
		} else if (ov == OverlayType.HORIZON_DETECTION) {
			vg = (ViewGroup) horizonDetection.getParent();
			vg.removeView(horizonDetection);
			horizonDetection = null;
		} else if (ov == OverlayType.MAIN_LINES) {
			vg = (ViewGroup) majorLines.getParent();
			vg.removeView(majorLines);
			majorLines = null;
		} else if (ov == OverlayType.OBJECT_SEGMENTATION) {
			vg = (ViewGroup) objectSegmentation.getParent();
			vg.removeView(objectSegmentation);
			objectSegmentation = null;
		}
	}
	
	public void addOverlay(OverlayType ov) {
		Overlay ovlay = null;
		if (ov == OverlayType.THIRDS_GRID) {
			ovlay = activeGrid = new ThirdsGrid(getContext(), getWidth(), getHeight());
		} else if (ov == OverlayType.GOLDEN_THIRDS_GRID) {
			ovlay = activeGrid = new GoldenGrid(getContext(), getWidth(), getHeight());
		} else if (ov == OverlayType.GOLDEN_TRIANGLE_GRID) {
			ovlay = activeGrid = new TriangleGrid(getContext(), getWidth(), getHeight());
		} else if (ov == OverlayType.FACE_DETECTION) {
			ovlay = faceDetection = new FaceDetection(act, activeGrid, previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.COLOR_HIST) {
			ovlay = colorHistogram = new ColorHistogram(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.COLOR_WHEEL) {
			ovlay = colorWheel = new ColorWheel(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.SATURATION_DETECTION) {
			ovlay = saturation = new SaturationDetection(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.HORIZON_DETECTION) {
			ovlay = horizonDetection = new HorizonDetection(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.MAIN_LINES) {
			ovlay = majorLines = new MainLinesDetection(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.OBJECT_SEGMENTATION) {
			ovlay = objectSegmentation = new ObjectSegmentation(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		}
		
		if (ovlay != null)
			act.addContentView(ovlay, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
		
		if(faceDetection != null) {
			faceDetection.setGrid(activeGrid);
		}
	}
}
