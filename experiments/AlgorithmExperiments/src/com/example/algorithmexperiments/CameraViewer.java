package com.example.algorithmexperiments;

import java.util.List;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.util.Log;
import android.view.View;

public class CameraViewer implements CvCameraViewListener2 {

	private ViewModes viewMode;
	private CameraBridgeViewBase camera;
	
	private List<Object> params;
	private Mat rgba, gray;
	public CameraViewer(View viewer){
		this.camera = (CameraBridgeViewBase) viewer;
		this.camera.setMaxFrameSize(800, 480);
		this.viewMode = null;
	}

	public void createViewer() {
		camera.setCvCameraViewListener(this);
		camera.enableFpsMeter();
	}
	
	public void startViewer() {
		if(camera != null) {
			camera.enableView();
		}
	}
	
	public void stopViewer() {
		if(camera != null) {
			camera.disableView();
		}
	}
	
	@Override
	public void onCameraViewStarted(int width, int height) {
		rgba = new Mat(width, height, CvType.CV_8UC4);
		gray = new Mat(width, height, CvType.CV_8UC1);
	}

	@Override
	public void onCameraViewStopped() {
	}

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		rgba = inputFrame.rgba();
		if(viewMode == ViewModes.CANNY) {
			gray = inputFrame.gray();
			Integer first_threshold = (Integer) params.get(0);
			Integer second_threshold = (Integer) params.get(1);
			rgba = ImageProcessing.canny_Java(rgba, gray, first_threshold, second_threshold);
		} else if(viewMode == ViewModes.CANNY_BILLATERAL) {
			gray = inputFrame.gray();
			Integer first_threshold = (Integer) params.get(0);
			Integer second_threshold = (Integer) params.get(1);
			Integer d = (Integer) params.get(2);
			Integer sigmaColor = (Integer) params.get(3);
			Integer sigmaSpace = (Integer) params.get(4);
			rgba = ImageProcessing.canny_bilateral_Java(rgba, gray, first_threshold, second_threshold, d, sigmaColor, sigmaSpace);
		} else if (viewMode == ViewModes.CANNY_JNI) {
			gray = inputFrame.gray();
			Integer first_threshold = (Integer) params.get(0);
			Integer second_threshold = (Integer) params.get(1);
			ImageProcessing.canny_JNI(rgba, gray, first_threshold, second_threshold);
		} else if (viewMode == ViewModes.HOUGH) {
			rgba = ImageProcessing.hough(inputFrame.rgba(), inputFrame.gray());
		} else if (viewMode == ViewModes.KMEANS) {
			ImageProcessing.kmeans(rgba);
		}
		return rgba;
	}

	public void setMode(ViewModes mode, List<Object> params) {
		viewMode = mode;
		this.params = params;
	}

    
}
