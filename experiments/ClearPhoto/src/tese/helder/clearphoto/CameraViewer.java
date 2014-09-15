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
import tese.helder.clearphoto.overlays.imageprocessing.ImageSimplicity;
import tese.helder.clearphoto.overlays.imageprocessing.ColorHistogram;
import tese.helder.clearphoto.overlays.imageprocessing.ColorTemplates;
import tese.helder.clearphoto.overlays.imageprocessing.ColorWheel;
import tese.helder.clearphoto.overlays.imageprocessing.FaceDetection;
import tese.helder.clearphoto.overlays.imageprocessing.HorizonDetection;
import tese.helder.clearphoto.overlays.imageprocessing.HueCount;
import tese.helder.clearphoto.overlays.imageprocessing.ImageBalance;
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
import android.os.Debug;
import android.util.Log;
import android.util.Pair;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.Toast;

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
	private ImageSimplicity bgSimplicity;
	private HueCount hueCount;
	private ImageBalance imageBalance;
	private ColorTemplates colorTemplate;

	private int previewWidth, previewHeight;
	private List<Pair<ImageProcessingOv, LayoutParams>> imageProcessingOv;

	private Activity act;

	public CameraViewer(Activity act) {
		super(act);
		this.act = act;
		imageProcessingOv = new ArrayList<Pair<ImageProcessingOv, LayoutParams>>();

		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed.
		mHolder = getHolder();
		mHolder.addCallback(this);
	}

	public void openCamera() {
		if(mCamera != null)
			return;
		try {
			mCamera = Camera.open(); // attempt to get a Camera instance
		}
		catch (Exception e){
			Log.e(TAG, "Failed to open camera: " + e.getMessage());
		}

		if(mCamera == null)
			return;
		setupCamera();
	}

	public void setupCamera() {
		if(mCamera == null)
			return;
		try {
			mCamera.setPreviewDisplay(mHolder);
		} catch (IOException e) {
			Log.d(TAG, "Error setting camera preview: " + e.getMessage());
		}
		// set preview size and make any resize, rotate or
		// reformatting changes here

		// start preview with new settings
		try {
			Camera.Parameters params = mCamera.getParameters();

			if(params.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)){
				// video mode focus is less aggressive than picture mode
				params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
				Log.d(TAG, "Camera focus mode: " + Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
			}
			//params.setSceneMode(Camera.Parameters.SCENE_MODE_NIGHT);
			//params.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);

			previewWidth = mCamera.getParameters().getPreviewSize().width;
			previewHeight = mCamera.getParameters().getPreviewSize().height;

			mCamera.setParameters(params);
			mCamera.setPreviewDisplay(mHolder);
			mCamera.setPreviewCallback(this);
			mCamera.startPreview();
		} catch (Exception e){
			Log.d(TAG, "Error starting camera preview: " + e.getMessage());
		}
	}

	public void releaseCamera() {
		if (mHolder.getSurface() == null){
			// preview surface does not exist
			return;
		}

		// stop preview before making changes
		try {
			mCamera.stopPreview();
			mCamera.setPreviewCallback(null);
			mCamera.release();
			mCamera = null;
		} catch (Exception e){
			// ignore: tried to stop a non-existent preview
		}
	}

	public void surfaceCreated(SurfaceHolder holder) {
		openCamera();
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		releaseCamera();
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		setupCamera();
	}

	private long dif = -1, startT = -1;
	private long testTime = 60000000000l;
	private int i = 1;
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		if(i > 10)
			i = 1;
		
		if(faceDetection != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m face detection",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("faceDetection");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m face detection",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("faceDetection"+i);
			faceDetection.process(data);
			android.os.Debug.stopMethodTracing();
			i++;
		}
		if(colorHistogram != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m color histogram",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("colorHistogram");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m color histogram",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("colorHistogram"+i);
			colorHistogram.process(data);
			android.os.Debug.stopMethodTracing();
			i++;
		}
		if(colorWheel != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m color wheel",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("colorWheel");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m color wheel",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("colorWheel"+i);
			colorWheel.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(saturation != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m saturation detection",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("saturationDetection");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m saturation detection",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("saturationDetection"+i);
			saturation.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(horizonDetection != null){
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m horizon detection",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("horizonDetection");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m horizon detection",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("horizonDetection"+i);
			horizonDetection.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(majorLines != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m major lines detection",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("majorLines");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m major lines detection",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("majorLines"+i);
			majorLines.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(objectSegmentation != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m object segmentation",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("objectSegmentation");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m object segmentation",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("objectSegmentation"+i);
			objectSegmentation.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(bgSimplicity != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m image simplicity",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("imageSimplicity");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m imageSimplicity",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("imageSimplicity"+i);
			bgSimplicity.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(hueCount != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m hue count",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("hueCount");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m hue count",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("hueCount"+i);
			hueCount.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(imageBalance != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m image balance",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("imageBalance");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m image balance",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("imageBalance"+i);
			imageBalance.process(data);
			android.os.Debug.stopMethodTracing();i++;
		}
		if(colorTemplate != null) {
//			if(startT== -1) {
//				Toast.makeText(act, "STARTED 1m colour template",Toast.LENGTH_LONG).show();
//				startT = System.nanoTime();
//				android.os.Debug.startMethodTracing("colorTemplate");
//			}
//			long dif = System.nanoTime() - startT;
//			if(dif >= testTime) {
//				android.os.Debug.stopMethodTracing();
//				Toast.makeText(act, "FINISHED 1m colour template",Toast.LENGTH_LONG).show();
//			}
			android.os.Debug.startMethodTracing("colorTemplate"+i);
			colorTemplate.process(data);
			android.os.Debug.stopMethodTracing();i++;
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
		} else if (ov == OverlayType.BACKGROUND_SIMPLICITY) {
			vg = (ViewGroup) bgSimplicity.getParent();
			vg.removeView(bgSimplicity);
			bgSimplicity = null;
		} else if (ov == OverlayType.HUE_COUNT) {
			vg = (ViewGroup) hueCount.getParent();
			vg.removeView(hueCount);
			hueCount = null;
		} else if (ov == OverlayType.IMAGE_BALANCE) {
			vg = (ViewGroup) imageBalance.getParent();
			vg.removeView(imageBalance);
			imageBalance = null;
		} else if (ov == OverlayType.COLOR_TEMPLATE) {
			vg = (ViewGroup) colorTemplate.getParent();
			vg.removeView(colorTemplate);
			colorTemplate = null;
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
		} else if (ov == OverlayType.BACKGROUND_SIMPLICITY) {
			ovlay = bgSimplicity = new ImageSimplicity(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.HUE_COUNT) {
			ovlay = hueCount = new HueCount(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.IMAGE_BALANCE) {
			ovlay = imageBalance = new ImageBalance(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		} else if (ov == OverlayType.COLOR_TEMPLATE) {
			ovlay = colorTemplate = new ColorTemplates(getContext(), previewWidth, previewHeight, getWidth(), getHeight());
		}

		if (ovlay != null)
			act.addContentView(ovlay, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

		if(faceDetection != null) {
			faceDetection.setGrid(activeGrid);
		}
	}
}
