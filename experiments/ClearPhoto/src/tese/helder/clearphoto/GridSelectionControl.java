package tese.helder.clearphoto;

import java.util.ArrayList;
import java.util.List;

import tese.helder.clearphoto.overlays.OverlayType;

import android.app.Activity;
import android.app.AlertDialog;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class GridSelectionControl extends BaseAdapter {

	private LayoutInflater inflater;

	private ListView list;
	private List<Pair<OverlayType, String>> data;

	private Activity act;
	private CameraViewer cameraViewer;
	
	private AlertDialog dialog;
	
	public GridSelectionControl(Activity act, CameraViewer viewer) {
		this.act = act;
		this.cameraViewer = viewer;
		
		this.inflater = LayoutInflater.from(act);
		
		this.data = new ArrayList<Pair<OverlayType,String>>();
		this.data.add(new Pair<OverlayType, String>(OverlayType.GRID_THIRDS, "Rule of Thirds"));
		this.data.add(new Pair<OverlayType, String>(OverlayType.GRID_THIRDS_GOLDEN, "Golden Thirds"));
		this.data.add(new Pair<OverlayType, String>(OverlayType.GRID_GOLDEN_TRIANGLES, "Golden Triangles"));
	}

	public void showDialog() {
		final View promptView = inflater.inflate(R.layout.grid_selection_dialog, null);	

		//Creates list to insert in prompt dialog
		list = (ListView) promptView.findViewById(R.id.grid_selection_list);
		list.setAdapter(this);
		list.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position,
					long id) {
				Pair<OverlayType, String> ov = data.get(position);
				cameraViewer.addOverlay(ov.first);
				dialog.dismiss();
			}
		});
		
		//Creates prompt dialog for controls
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(act);
		alertDialogBuilder.setView(promptView);
		dialog = alertDialogBuilder.create();
		dialog.show();
	}
	
	public int getCount() {
		return data.size();
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View vi = convertView;
		if(convertView == null)
			vi = inflater.inflate(R.layout.grid_selection_row, null);
		
		TextView name = (TextView) vi.findViewById(R.id.grid_selection_title);
		name.setText(data.get(position).second);
		return vi;
	}
}
