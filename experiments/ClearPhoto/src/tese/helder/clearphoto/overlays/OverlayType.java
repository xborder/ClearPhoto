package tese.helder.clearphoto.overlays;

public enum OverlayType {
	GRID, THIRDS_GRID, GOLDEN_THIRDS_GRID, GOLDEN_TRIANGLE_GRID,
	FACE_DETECTION, COLOR_HIST, COLOR_WHEEL, SATURATION_DETECTION,
	HORIZON_DETECTION, MAIN_LINES, OBJECT_SEGMENTATION, BACKGROUND_SIMPLICITY,
	HUE_COUNT, IMAGE_BALANCE, COLOR_TEMPLATE,
	DIV1, DIV2, DIV3;

	@Override
	public String toString() {
		switch(this) {
		case DIV1:					return "Grids";
		case DIV2:					return "Colour Info";
		case DIV3:					return "Detectors";
		case GRID: 					return "";
		case THIRDS_GRID: 			return "Rule of Thirds Grid";
		case GOLDEN_THIRDS_GRID: 	return "Golden Rule of Thirds Grid";
		case GOLDEN_TRIANGLE_GRID: 	return "";
		case FACE_DETECTION: 		return "Face detection";
		case COLOR_HIST: 			return "Colour Histogram";
		case COLOR_WHEEL: 			return "Colour Wheel";
		case SATURATION_DETECTION: 	return "Saturation Detection";
		case HORIZON_DETECTION: 	return "Horizon Detection";
		case MAIN_LINES: 			return "Main lines Detection";
		case OBJECT_SEGMENTATION: 	return "Generic Object Segmentation";
		case HUE_COUNT: 			return "Hue Count";
		case IMAGE_BALANCE: 		return "Image Balance Detection";
		case COLOR_TEMPLATE: 		return "Color Template Detection";
		case BACKGROUND_SIMPLICITY: return "Image Simplicity";
		default: throw new IllegalArgumentException();
		}
	}
}
