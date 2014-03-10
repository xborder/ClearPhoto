package com.example.algorithmexperiments;

import java.util.ArrayList;
import java.util.List;

import org.opencv.android.OpenCVLoader;
import org.opencv.imgproc.Imgproc;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.Fragment;
import android.content.DialogInterface;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RadioGroup;
import android.widget.TextView;

public class MainActivity extends Activity {

	static {
		if (!OpenCVLoader.initDebug()) {
		} else {
			System.loadLibrary("algorithms");
		}
	}

	private CameraViewer cameraViewer;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.activity_main);

		cameraViewer = new CameraViewer(findViewById(R.id.camera_surface_view));
		cameraViewer.createViewer();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.canny:
			canny_handle();
			return true;
		case R.id.canny_jni:
			canny_jni_handle();
			return true;
		case R.id.hough:
			hough_handle();
			return true;
		case R.id.kmeans:
			cameraViewer.setMode(ViewModes.KMEANS, null);
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}
	@Override
	protected void onResume() {
		super.onResume();
		cameraViewer.startViewer();
	}

	@Override
	protected void onPause() {
		super.onPause();
		cameraViewer.stopViewer();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		cameraViewer.stopViewer();
	}
	
	private void hough_handle() {
		cameraViewer.setMode(ViewModes.HOUGH, null);
	}

	private void canny_jni_handle() {
		LayoutInflater layoutInflater = LayoutInflater.from(this);

		final View promptView = layoutInflater.inflate(R.layout.canny_jni, null);

		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);

		// set prompts.xml to be the layout file of the alertdialog builder
		alertDialogBuilder.setView(promptView);

		// setup a dialog window
		alertDialogBuilder
		.setCancelable(false)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				List<Object> params = new ArrayList<Object>();
				
				TextView t1 = (TextView) promptView.findViewById(R.id.jni_editText1);
				TextView t2 = (TextView) promptView.findViewById(R.id.jni_editText2);
				
				Integer first_threshold = Integer.parseInt(t1.getText().toString());
				Integer second_threshold = Integer.parseInt(t2.getText().toString());
				
				
				params.add(first_threshold);
				params.add(second_threshold);
				
				cameraViewer.setMode(ViewModes.CANNY_JNI, params);
			}
		})
		.setNegativeButton("Cancel",
				new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,	int id) {
				dialog.cancel();
			}
		});

		// create an alert dialog
		AlertDialog alertD = alertDialogBuilder.create();

		alertD.show();
	}
	
	private void canny_handle() {
		LayoutInflater layoutInflater = LayoutInflater.from(this);

		final View promptView = layoutInflater.inflate(R.layout.canny, null);

		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);

		// set prompts.xml to be the layout file of the alertdialog builder
		alertDialogBuilder.setView(promptView);

		// setup a dialog window
		alertDialogBuilder
		.setCancelable(false)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				List<Object> params = new ArrayList<Object>();

				TextView t1 = (TextView) promptView.findViewById(R.id.editText1);
				TextView t2 = (TextView) promptView.findViewById(R.id.editText2);
				RadioGroup r = (RadioGroup) promptView.findViewById(R.id.radioGroup1);

				Integer first_threshold = Integer.parseInt(t1.getText().toString());
				Integer second_threshold = Integer.parseInt(t2.getText().toString());

				Boolean normal = r.getCheckedRadioButtonId() == R.id.radio0;

				params.add(first_threshold);
				params.add(second_threshold);

				if(normal) {
					cameraViewer.setMode(ViewModes.CANNY, params);
				} else {
					Integer d = Integer.parseInt(((TextView) promptView.findViewById(R.id.editText3)).getText().toString());
					Integer bil_t1 = Integer.parseInt(((TextView) promptView.findViewById(R.id.editText4)).getText().toString());
					Integer bil_t2 = Integer.parseInt(((TextView) promptView.findViewById(R.id.editText5)).getText().toString());

					params.add(d);
					params.add(bil_t1);
					params.add(bil_t2);
					cameraViewer.setMode(ViewModes.CANNY_BILLATERAL, params);
				}
			}
		})
		.setNegativeButton("Cancel",
				new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,	int id) {
				dialog.cancel();
			}
		});

		// create an alert dialog
		AlertDialog alertD = alertDialogBuilder.create();

		alertD.show();
	}
}