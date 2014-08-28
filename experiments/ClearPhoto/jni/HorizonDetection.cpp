#include "HorizonDetection.h"
#include <cassert>
HorizonDetection::HorizonDetection() {
	canny_min_threshold = 100;
	canny_max_threshold = canny_min_threshold*2;

	median_rho = median_theta = 0;
}

HorizonDetection::~HorizonDetection() { }
//TODO Simplificar para uma linha só
void HorizonDetection::horizonEdgeColorDetection(Mat* data, Mat* canny, int* ret) {
	int* color_output = (int*)calloc(6*N_HORIZON_RESULTS, sizeof(int)); //x0,y0,x1,y1,theta,rho
	int* edge_output = (int*)calloc(6*N_HORIZON_RESULTS, sizeof(int)); //x0,y0,x1,y1,theta,rho

	horizonColorDetection(data, canny, color_output, false);
	Point p1,p2;
	for(int i = 0; i < 6*N_HORIZON_RESULTS; i+=6) {
		p1.x += color_output[i];
		p1.y += color_output[i+1];
		p2.x += color_output[i+2];
		p2.y += color_output[i+3];
	}
	p1.x /= N_HORIZON_RESULTS;
	p1.y /= N_HORIZON_RESULTS;
	p2.x /= N_HORIZON_RESULTS;
	p2.y /= N_HORIZON_RESULTS;
	if(color_output[0] != -1) {
		median_theta = color_output[4];
		median_rho = color_output[5];

		horizonEdgeDetection(data, canny, edge_output, true);

		Point pe1,pe2;
		for(int i = 0; i < 6*N_HORIZON_RESULTS; i+=6) {
			pe1.x += edge_output[i];
			pe1.y += edge_output[i+1];
			pe2.x += edge_output[i+2];
			pe2.y += edge_output[i+3];
		}
		pe1.x /= N_HORIZON_RESULTS;
		pe1.y /= N_HORIZON_RESULTS;
		pe2.x /= N_HORIZON_RESULTS;
		pe2.y /= N_HORIZON_RESULTS;

		cout << pe1 << " " << pe2 << endl;

		Point firstMedianP;
		double firstMedianM = (p1.x == p2.x) ? 0.0 : (p1.y - p2.y)/(p1.x - p2.x);
		firstMedianP.x = (p1.x + p2.x)/2;
		firstMedianP.y = (p1.y + p2.y)/2;

		Point secMedianP;
		double secMedianM = (pe1.x == pe2.x) ? 0.0 : (pe1.y - pe2.y)/(pe1.x - pe2.x);
		secMedianP.x = (pe1.x + pe2.x)/2;
		secMedianP.y = (pe1.y + pe2.y)/2;

		Point avgPoint;
		double avgM = (firstMedianM + secMedianM)/2;
		avgPoint.x = (firstMedianP.x + secMedianP.x)/2;
		avgPoint.y = (firstMedianP.y + secMedianP.y)/2;

		Point ext1(-1,-1), ext2(-1,-1);
		int calc = (0 - avgPoint.y + avgM * avgPoint.x)/avgM;
		if( calc >= 0 && calc <= data->cols ) {
			ext1 = Point(calc, 0);
		}
		calc = (data->rows - avgPoint.y + avgM * avgPoint.x)/avgM;
		if( calc >= 0 && calc <= data->cols ) {
			if(avgPoint.x == -1 && avgPoint.y == -1)
				ext1 = Point(calc, data->rows);
			else
				ext2 = Point(calc, data->rows);
		}
		calc = avgM * 0 - avgM * avgPoint.x + avgPoint.y;
		if( calc >= 0 && calc <= data->rows ) {
			if(ext1.x == -1 && ext1.y == -1)
				ext1 = Point(0, calc);
			else
				ext2 = Point(0, calc);
		}
		calc = avgM * data->cols - avgM * avgPoint.x + avgPoint.y;
		if( calc >= 0 && calc <= data->rows ) {
			if(ext1.x == -1 && ext1.y == -1)
				ext1 = Point(data->cols, calc);
			else
				ext2 = Point(data->cols, calc);
		}
		int result[12]= {p1.x, p1.y, p2.x, p2.y, pe1.x, pe1.y, pe2.x, pe2.y, ext1.x, ext1.y, ext2.x, ext2.y};
		memmove(ret, result, 12*sizeof(int));
	}
	free(color_output);
	free(edge_output);
}

void HorizonDetection::horizonEdgeDetection(Mat* data, Mat* canny, int* ret, bool intensify) {
	Mat rgb, yuv;
	cvtColor(*data, rgb, CV_YUV420sp2BGR);
	cvtColor(rgb, yuv, CV_BGR2YUV);
	vector<Mat> yuv_planes;
	split(yuv, yuv_planes );

	Mat luma;
	luma = yuv_planes[0];

	/// Detect edges using canny
	Mat canny_output, blur;

	cv::blur( luma, blur, Size(5,5) );
	Canny( blur, canny_output, canny_min_threshold, canny_max_threshold, 3 );

	// Closing operation, quanto maior o kernel, mais dispersos ficam os horizontes
	Mat closing_output;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
	morphologyEx(canny_output, closing_output, MORPH_CLOSE, kernel);

	memcpy(canny->data, closing_output.data, sizeof(uchar)*canny->cols*canny->rows);
	//Hough transformation
	getLines(closing_output, ret, intensify);
}

void HorizonDetection::horizonColorDetection(Mat* data, Mat* canny, int* ret, bool intensify) {
	Mat rgb, yuv;
	cvtColor(*data, rgb, CV_YUV420sp2BGR);
	cvtColor(rgb, yuv, CV_BGR2YUV);
	vector<Mat> yuv_planes;
	split(yuv, yuv_planes );

	Mat sky_filter = Mat::zeros(yuv.rows, yuv.cols, CV_8UC1);

	int YM = 210, UM = 150, VM = 100;
	int VARY = 130, VARU = 40, VARV = 40;
	for(int i = 0;i < yuv.rows;i++){
		double P = pow((double)i / yuv.rows, 2);
		double positionP = exp(-P);
		for(int j = 0;j < yuv.cols;j++){
			uchar y = yuv_planes[0].at<uchar>(i,j);
			uchar u = yuv_planes[1].at<uchar>(i,j);
			uchar v = yuv_planes[2].at<uchar>(i,j);
			double A = (y-YM)/VARY;
			double B = (u-UM)/VARU;
			double C = (v-VM)/VARV;
			double Z = (pow(A,2)+ pow(B,2) + pow(C,2));
			double colorP = exp(-Z);

			/*
			int t = 0;
			for(int k = -2; k < 2; k++) {
				if(j-k < 0 || j+k >= yuv.cols)
					continue;
				t += abs(y - yuv_planes[0].at<uchar>(i,j + k));
			}

			int t0 = 10;
			double textureP = (t > t0) ? exp(-0.2 * pow((double)(t - t0), 2)) : 1;
			 */


			int t = 0;
			if(j+1 < yuv.cols)
				t = abs(y - yuv_planes[0].at<uchar>(i,j + 1));
			double textureP = exp(-0.2 * pow((double)(t), 2));
			double Psky = (positionP*colorP*textureP) ;

			sky_filter.at<uchar>(i,j) = (Psky * 100 > SKY_THRESHOLD) ? 255 : 0;
		}
	}
	Mat canny_output;
	Canny(sky_filter, canny_output, canny_min_threshold, canny_max_threshold, 3 );

	memcpy(canny->data, canny_output.data, sizeof(uchar)*canny_output.cols*canny_output.rows);
	getLines(canny_output, ret, intensify);
}

void HorizonDetection::getLines(Mat src, int* ret, bool intensify) {
	Size img_size(src.cols, src.rows);

	double hough_h = ((sqrt(2.0) * (double)(img_size.height > img_size.width ? img_size.height : img_size.width)) / 2.0); // finding out max rho
	Size acum_size(180, hough_h * 2.0); // -rho -> +rho, 0 -> 180

	double center_x = img_size.width/2;
	double center_y = img_size.height/2;
	int threshold = img_size.width > img_size.height ? img_size.width/4 : img_size.height/4;

	int* acum = (int*)calloc(acum_size.width * acum_size.height, sizeof(int));
	int count = 0, rho_sum = 0, theta_sum = 0;
	vector<int> rhos, thetas;
	for(int y=0; y < img_size.height; y++) {
		for(int x=0 ; x < img_size.width ; x++) {
			if( src.data[ (y * img_size.width) + x] > 250 ) {
				for(int t=0;t<180;t++) {
					double r = ( ((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));
					acum[(int)(round(r + hough_h) * 180.0) + t] ++;
					theta_sum += t; rho_sum += r; count ++;
					rhos.push_back((int)r);
					thetas.push_back(t);
				}
			}
		}
	}

	double rho_deviation = 0.0;
	double theta_deviation = 0.0;
	if(count == 0) return;
	if(intensify) {
		int rhos_avg = rho_sum/count;
		int thetas_avg = theta_sum/count;

		rho_deviation = houghSpaceDeviation(rhos, rhos_avg);
		theta_deviation = houghSpaceDeviation(thetas, thetas_avg);
		for(int r = median_rho - 10; r < median_rho + 10 ; r++) {
			if(r < 0 || r > acum_size.height)
				continue;
			for(int t = median_theta - 10; t < median_theta + 10; t++) {
				if(t < 0 || t > acum_size.width)
					continue;
				double factor = gaussian2D(r,t,rho_deviation,theta_deviation);
				//printf("before: %d factor: %f after: %f\n", acum[(r*acum_size.width) + t], factor, (acum[(r*acum_size.width) + t] + acum[(r*acum_size.width) + t]*factor));
			}
		}
	}

	int i = 0, offset = 0, last_max = INT_MAX, last_rho = INT_MAX, last_theta = INT_MAX;
	int maxmax = 0;
	while(i < N_HORIZON_RESULTS) {
		int max_val = 0, max_rho = -1, max_theta = -1;
		for(int r=0; r < acum_size.height ; r++) {
			for(int t=0; t < acum_size.width; t++) {
				if(acum[(r*acum_size.width) + t] > max_val && acum[(r*acum_size.width) + t]  < last_max && r != last_rho && t != last_theta) {
					max_rho = r;
					max_theta = t;
					max_val = acum[(r*acum_size.width) + t];
					if(max_val > maxmax)
						maxmax = max_val;
				}
			}
		}

		if(max_rho == -1 && max_theta == -1) {
			ret[0] = ret[1] = ret[2] = ret[3] = ret[4] = ret[5] = -1;
			i++;
			continue;
		}

		int t = max_theta, r = max_rho;

		int x1, y1, x2, y2;
		x1 = y1 = x2 = y2 = 0;

		if(t >= 45 && t <= 135) {
			//y = (r - x cos(t)) / sin(t)
			x1 = 0;
			y1 = ((double)(r-(acum_size.height/2)) - ((x1 - (img_size.width/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (img_size.height / 2);
			x2 = img_size.width;
			y2 = ((double)(r-(acum_size.height/2)) - ((x2 - (img_size.width/2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (img_size.height / 2);
		} else {
			//x = (r - y sin(t)) / cos(t);
			y1 = 0;
			x1 = ((double)(r-(acum_size.height/2)) - ((y1 - (img_size.height/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (img_size.width / 2);
			y2 = img_size.height;
			x2 = ((double)(r-(acum_size.height/2)) - ((y2 - (img_size.height/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (img_size.width / 2);
		}
		ret[offset++] = x1;
		ret[offset++] = y1;
		ret[offset++] = x2;
		ret[offset++] = y2;
		ret[offset++] = t;
		ret[offset++] = r;

		last_max = max_val;
		last_rho = max_rho;
		last_theta = max_theta;
		i++;
	}

	free(acum);
}

double HorizonDetection::houghSpaceDeviation(vector<int> v, double ave) {
	double E=0;
	int size = v.size();
	for(int i=0;i<size;i++)
		E+=(v[i] - ave)*(v[i] - ave);
	return sqrt(1/(double)size*E);
}

double HorizonDetection::gaussian2D(int rho, int theta, double rho_deviation, double theta_deviation) {
	return exp(-(pow((double)rho - median_rho,2)/(2*pow(theta_deviation,2)) + pow((double)theta - median_theta,2)/(2*pow(theta_deviation,2))));
}
