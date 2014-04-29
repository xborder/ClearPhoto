package tese.helder.clearphoto.overlays.grids;

import java.util.ArrayList;
import java.util.List;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.R;
import tese.helder.clearphoto.overlays.OverlayType;

import android.app.AlertDialog;
import android.content.Context;
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

	private CameraViewer cameraViewer;
	private AlertDialog dialog;
	private Context context;
	
	public GridSelectionControl(Context parentContext, CameraViewer parentViewer) {
		this.cameraViewer = parentViewer;
		this.context = parentContext;
		this.inflater = LayoutInflater.from(parentContext);
		
		this.data = new ArrayList<Pair<OverlayType,String>>();
		this.data.add(new Pair<OverlayType, String>(OverlayType.THIRDS_GRID, "Rule of Thirds"));
		this.data.add(new Pair<OverlayType, String>(OverlayType.GOLDEN_THIRDS_GRID, "Golden Thirds"));
		this.data.add(new Pair<OverlayType, String>(OverlayType.GOLDEN_TRIANGLE_GRID, "Golden Triangles"));
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
				Pair<OverlayType, String> ov = data.get(position);
				cameraViewer.addOverlay(ov.first);
				dialog.dismiss();
			}
		});
		
		//Creates prompt dialog for controls
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
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
