package tese.helder.clearphoto;

import java.util.ArrayList;
import java.util.List;

import tese.helder.clearphoto.overlays.OverlayType;
import android.app.Activity;
import android.app.AlertDialog;
import android.util.Pair;
import android.view.LayoutInflater;
import android.widget.ListView;

public class OptionsView {

	private LayoutInflater inflater;
	private Activity act;
	private CameraViewer cameraViewer;
	
	public OptionsView(Activity act, CameraViewer viewer) {
		this.act = act;
		this.cameraViewer = viewer;
		this.inflater = LayoutInflater.from(act);
	}
	
	public void inflate() {
		
	}
}
