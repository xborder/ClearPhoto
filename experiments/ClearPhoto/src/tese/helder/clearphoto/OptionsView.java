package tese.helder.clearphoto;

import java.util.ArrayList;
import java.util.List;

import tese.helder.clearphoto.overlays.OverlayType;
import tese.helder.clearphoto.overlays.grids.Grid;
import tese.helder.clearphoto.overlays.imageprocessing.BackgroundSimplicity;
import tese.helder.clearphoto.overlays.imageprocessing.ColorHistogram;
import tese.helder.clearphoto.overlays.imageprocessing.ColorTemplates;
import tese.helder.clearphoto.overlays.imageprocessing.ColorWheel;
import tese.helder.clearphoto.overlays.imageprocessing.FaceDetection;
import tese.helder.clearphoto.overlays.imageprocessing.HorizonDetection;
import tese.helder.clearphoto.overlays.imageprocessing.HueCount;
import tese.helder.clearphoto.overlays.imageprocessing.ImageBalance;
import tese.helder.clearphoto.overlays.imageprocessing.MainLinesDetection;
import tese.helder.clearphoto.overlays.imageprocessing.ObjectSegmentation;
import tese.helder.clearphoto.overlays.imageprocessing.SaturationDetection;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.AbsListView;
import android.widget.AbsListView.OnScrollListener;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

public class OptionsView extends BaseAdapter{
	private static OverlayType[] ovs = {
		OverlayType.THIRDS_GRID, OverlayType.GOLDEN_THIRDS_GRID,
		OverlayType.FACE_DETECTION, OverlayType.COLOR_HIST, OverlayType.COLOR_WHEEL, OverlayType.COLOR_TEMPLATE,
		OverlayType.HUE_COUNT, OverlayType.SATURATION_DETECTION,
		OverlayType.HORIZON_DETECTION, OverlayType.MAIN_LINES, OverlayType.OBJECT_SEGMENTATION, OverlayType.BACKGROUND_SIMPLICITY,
		OverlayType.IMAGE_BALANCE};
	private class ViewHolder {
		CheckBox cb;
		OverlayType ov;
		boolean isSelected;
	}

	private LayoutInflater inflater;

	private ListView list;
	private List<ViewHolder> viewHolders;

	private CameraViewer cameraViewer;
	private AlertDialog dialog;
	private Context context;

	public OptionsView(Context parentContext, CameraViewer parentViewer) {
		this.cameraViewer = parentViewer;
		this.context = parentContext;
		this.inflater = LayoutInflater.from(parentContext);

		this.viewHolders = new ArrayList<ViewHolder>();
		for(int i = 0; i < ovs.length; i++) {
			ViewHolder vh = new ViewHolder();
			vh.cb = null;
			vh.isSelected = false;
			vh.ov = ovs[i];
			this.viewHolders.add(vh);
		}
	}

	public void inflate() {
		final View promptView = inflater.inflate(R.layout.grid_selection_dialog, null);	
		//Creates list to insert in prompt dialog
		list = (ListView) promptView.findViewById(R.id.grid_selection_list);
		list.setAdapter(this);
		list.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position,
					long id) {
				ViewHolder vh = viewHolders.get(position);
				vh.isSelected = !vh.isSelected;
				vh.cb.setChecked(vh.isSelected);
				if(vh.isSelected)
					cameraViewer.addOverlay(vh.ov);
				else
					cameraViewer.removeOverlay(vh.ov);
			}
		});

		//Creates prompt dialog for controls
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
		alertDialogBuilder.setView(promptView);
		dialog = alertDialogBuilder.create();
		dialog.setCanceledOnTouchOutside(true);
		dialog.show();
	}

	public int getCount() {
		return viewHolders.size();
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(final int position, View convertView, ViewGroup parent) {

		ViewHolder holder = null;

		LayoutInflater vi = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		convertView = vi.inflate(R.layout.options_selection_row, null);

		holder = viewHolders.get(position);
		holder.cb = (CheckBox) convertView.findViewById(R.id.checkBox1);
		convertView.setTag(holder);
		holder.cb.setOnClickListener(new View.OnClickListener() {  
			public void onClick(View v) {  
				ViewHolder vh = viewHolders.get(position);
				vh.isSelected = !vh.isSelected;
				vh.cb.setChecked(vh.isSelected);
				if(vh.isSelected)
					cameraViewer.addOverlay(vh.ov);
				else
					cameraViewer.removeOverlay(vh.ov);
			}  
		});  

		holder.cb.setText(holder.ov.toString());
		holder.cb.setChecked(holder.isSelected);
		return convertView;
	}
}
