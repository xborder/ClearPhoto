package org.opencv.samples.tutorial2;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;

public class Tutorial2Activity extends Activity implements CvCameraViewListener2 {
    private static final String    TAG = "OCVSample::Activity";

    private static final int       	VIEW_MODE_RGBA     			= 0;
    private static final int       	VIEW_MODE_GRAY     			= 1;
    private static final int       	VIEW_MODE_CANNY    			= 2;
    private static final int       	VIEW_MODE_CANNY_BILATERAL	= 3;
    private static final int		VIEW_MODE_HOUGH	   			= 4;
    private static final int		VIEW_MODE_MEAN	   			= 5;
    private static final int       	VIEW_MODE_AVERAGE  			= 6;
    private static final int       	VIEW_MODE_WATERSHED			= 7;

    private int                    mViewMode;
    private Mat                    mRgba;
    private Mat                    mIntermediateMat;
    private Mat                    mGray;
    private Mat					   mHSV;
    
    private MenuItem               mItemPreviewRGBA;
    private MenuItem               mItemPreviewGray;
    private MenuItem               mItemPreviewCanny;
    private MenuItem               mItemPreviewCannyBilateral;
    private MenuItem               mItemPreviewHough;
    private MenuItem               mItemPreviewMeanShift;
    private MenuItem               mItemPreviewAverage;
    private MenuItem               mItemPreviewWatershed;

    private CameraBridgeViewBase   mOpenCvCameraView;

    static {
        if (!OpenCVLoader.initDebug()) {
            // Handle initialization error
        } else {
            System.loadLibrary("mixed_sample");
        }
    }
    
    public Tutorial2Activity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.tutorial2_surface_view);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.tutorial2_activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.enableFpsMeter();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "called onCreateOptionsMenu");
        mItemPreviewRGBA = menu.add("Preview RGBA");
        mItemPreviewGray = menu.add("Preview GRAY");
        mItemPreviewCanny = menu.add("Canny");
        mItemPreviewCannyBilateral = menu.add("Canny w/ bilateral");
        mItemPreviewHough = menu.add("Hough");
        mItemPreviewMeanShift = menu.add("MeanShift");
        mItemPreviewAverage = menu.add("Average HSV");
        mItemPreviewWatershed = menu.add("Watershed");
        return true;
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        mOpenCvCameraView.enableView();
    }

    public void onDestroy() {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mRgba = new Mat(height, width, CvType.CV_8UC4);
        mIntermediateMat = new Mat(height, width, CvType.CV_8UC4);
        mGray = new Mat(height, width, CvType.CV_8UC1);
        mHSV = new Mat();
    }

    public void onCameraViewStopped() {
        mRgba.release();
        mGray.release();
        mIntermediateMat.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        final int viewMode = mViewMode;
        switch (viewMode) {
        case VIEW_MODE_GRAY:
            // input frame has gray scale format
            Imgproc.cvtColor(inputFrame.gray(), mRgba, Imgproc.COLOR_GRAY2RGBA, 4);
            break;
        case VIEW_MODE_RGBA:
            // input frame has RBGA format
            mRgba = inputFrame.rgba();
            break;
        case VIEW_MODE_CANNY:
            // input frame has gray scale format
            mRgba = inputFrame.rgba();
            Imgproc.Canny(inputFrame.gray(), mIntermediateMat, 80, 100);
            Imgproc.cvtColor(mIntermediateMat, mRgba, Imgproc.COLOR_GRAY2RGBA, 4);
            break;
        case VIEW_MODE_CANNY_BILATERAL:

            mRgba = inputFrame.rgba();
            Imgproc.bilateralFilter(inputFrame.gray(), mGray, 5, 150, 150);
            Imgproc.Canny(mGray, mIntermediateMat, 80, 100);
            Imgproc.cvtColor(mIntermediateMat, mRgba, Imgproc.COLOR_GRAY2RGBA, 4);
        	break;
        case VIEW_MODE_HOUGH:
        	mRgba = inputFrame.rgba();
        	mGray = inputFrame.gray();
        	Mat lines = new Mat(mRgba.cols(), mRgba.rows(), CvType.CV_8UC4);
        	
        	Imgproc.Canny(mGray, mIntermediateMat, 50, 200);
        	Imgproc.HoughLinesP(mIntermediateMat, lines, 1, Math.PI/180, 50, 50, 10);
        	
            for (int x = 0; x < lines.cols(); x++) 
            {
                  double[] vec = lines.get(0, x);
                  double x1 = vec[0], 
                         y1 = vec[1],
                         x2 = vec[2],
                         y2 = vec[3];
                  Point start = new Point(x1, y1);
                  Point end = new Point(x2, y2);

                  Core.line(mRgba, start, end, new Scalar(255,0,0), 3);
            }
        	break;
        case VIEW_MODE_AVERAGE:
            // input frame has RGBA format
            mRgba = inputFrame.rgba();
            mGray = inputFrame.gray();

            Imgproc.cvtColor(mRgba, mHSV, Imgproc.COLOR_RGB2HSV);
            CalcExposure(mHSV.getNativeObjAddr(), mRgba.getNativeObjAddr());
            break;
        case VIEW_MODE_MEAN:
        	mRgba = inputFrame.rgba();
        	Imgproc.cvtColor(mRgba, mIntermediateMat, Imgproc.COLOR_RGBA2RGB);
        	Imgproc.pyrMeanShiftFiltering(mIntermediateMat, mRgba, 2, 2, 1, new TermCriteria());
        	break;
        case VIEW_MODE_WATERSHED:
        	mRgba = inputFrame.rgba();
        	WaterShed(mRgba.getNativeObjAddr());
        }

        return mRgba;
    }

    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "called onOptionsItemSelected; selected item: " + item);

        if (item == mItemPreviewRGBA) {
            mViewMode = VIEW_MODE_RGBA;
        } else if (item == mItemPreviewGray) {
            mViewMode = VIEW_MODE_GRAY;
        } else if (item == mItemPreviewCanny) {
            mViewMode = VIEW_MODE_CANNY;
        } else if (item == mItemPreviewCannyBilateral) {
            mViewMode = VIEW_MODE_CANNY_BILATERAL;
        } else if (item == mItemPreviewHough) {
            mViewMode = VIEW_MODE_HOUGH;
        } else if (item == mItemPreviewAverage) {
            mViewMode = VIEW_MODE_AVERAGE;
        } else if (item == mItemPreviewMeanShift) {
        	mViewMode = VIEW_MODE_MEAN;
        } else if (item == mItemPreviewWatershed) {
        	mViewMode = VIEW_MODE_WATERSHED;
        }

        return true;
    }

    public native void WaterShed(long matAddrRgba);
    public native void CalcExposure(long matAddrGr, long matAddrRgba);
    
}
