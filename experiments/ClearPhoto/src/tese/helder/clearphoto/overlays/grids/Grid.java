package tese.helder.clearphoto.overlays.grids;

import java.util.ArrayList;

import tese.helder.clearphoto.CameraViewer;
import tese.helder.clearphoto.overlays.Overlay;
import tese.helder.clearphoto.overlays.OverlayType;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RadialGradient;
import android.graphics.Shader;
import android.view.HapticFeedbackConstants;
import android.view.View;

public abstract class Grid extends Overlay {

	protected Paint gridColor;
	protected float[] powerPoints;
	protected float[] points;
	protected float[] nearest;
	protected boolean highlightPoints;
	protected ArrayList<Path> pathsToDraw;
	protected GridSelectionControl gridSelection;
	
	public Grid(Context context, int width, int height) {
		super(context, width, height);
		this.gridColor = new Paint();
		this.gridColor.setStyle(Paint.Style.FILL);
		this.gridColor.setColor(Color.WHITE);
		this.gridColor.setStrokeWidth(3f);

		this.pathsToDraw = new ArrayList<Path>();
	}

	@Override
	protected abstract void onDraw(Canvas canvas);

	public float[] getPowerPoints() {
		return this.powerPoints;
	}

	public void highligthPowerPoint(float[] nearest) {
		if(nearest == null) {
			this.highlightPoints = false;
		} else {
			this.highlightPoints = true;
			this.nearest = nearest;
			createOverlayIntersections();
		}
		this.invalidate();
	}
	
	private void createOverlayIntersections() {
		pathsToDraw.clear();
		for(int i = 0 ; i < nearest.length; i+=2) {
			Path pth = new Path();
			pth.moveTo(nearest[i]-1.5f,nearest[i+1]-height/6);
			pth.lineTo(nearest[i]+1.5f,nearest[i+1]-height/6);
			pth.lineTo(nearest[i]+1.5f,nearest[i+1]+height/6*3);
			pth.lineTo(nearest[i]-1.5f,nearest[i+1]+height/6*3);
			pth.lineTo(nearest[i]-1.5f,nearest[i+1]-height/6);


			Path pth2 = new Path();
			pth2.moveTo(nearest[i]-width/6,nearest[i+1]-1.5f);
			pth2.lineTo(nearest[i]+width/6,nearest[i+1]-1.5f);
			pth2.lineTo(nearest[i]+width/6,nearest[i+1]+1.5f);
			pth2.lineTo(nearest[i]-width/6,nearest[i+1]+1.5f);
			pth2.lineTo(nearest[i]-width/6,nearest[i+1]-1.5f);

			pathsToDraw.add(pth);
			pathsToDraw.add(pth2);
		}
	}
	
	protected void drawSuggestivePowerPoints(Canvas canvas) {
		if(highlightPoints && nearest != null) {
			for(int i = 0, j = 0; i < pathsToDraw.size() && j < nearest.length; i+=2) {
				Paint pa = new Paint(Paint.ANTI_ALIAS_FLAG|Paint.FILTER_BITMAP_FLAG);
				pa.setColor(0xff800000);
				pa.setShader(new RadialGradient(nearest[j++],nearest[j++],100,Color.RED, Color.TRANSPARENT, Shader.TileMode.CLAMP));
				canvas.drawPath(pathsToDraw.get(i),pa);	
				canvas.drawPath(pathsToDraw.get(i+1),pa);
			}
		}
	}
	
	protected void drawSuggestiveComposition(Canvas canvas) {
		if(nearest != null && nearest.length/2 == 3) {
			Paint pa = new Paint();
			pa.setStyle(Paint.Style.FILL);
			pa.setColor(Color.CYAN);
			pa.setAlpha(150);
			pa.setStrokeWidth(3f);
			float x0 = nearest[0], y0 = nearest[1];
			for(int i = 0; i < nearest.length; i+=2) {
				canvas.drawLine(x0, y0, nearest[i], nearest[i+1], pa);
				x0 = nearest[i]; y0 = nearest[i+1];
			}
			canvas.drawLine(x0, y0, nearest[0], nearest[1], pa);
		}
	}
	
	public static OnClickListener getOnClickListener(final Context parentContext, final CameraViewer parent) {
		return new OnClickListener() {
			private boolean inflated;
			@Override
			public void onClick(View v) {
				if(inflated) {
					parent.removeOverlay(OverlayType.GRID);
				} else {
					GridSelectionControl gridControl = new GridSelectionControl(parentContext, parent);
					gridControl.inflate();
				}
				inflated = !inflated;
				v.performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		};
	}
	
}
