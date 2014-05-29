#include "MainLinesDetection.h"

MainLinesDetection::MainLinesDetection() {}

MainLinesDetection::~MainLinesDetection() {}

vector< pair< pair<int, int>, pair<int, int> > > MainLinesDetection::getLines(Mat data, int threshold) {
	Mat rgb, yuv;
	cvtColor(data, rgb, CV_YUV420sp2BGR);

	/// Detect edges using canny
	Mat canny_output, blur;

	cv::blur( rgb, blur, Size(5,5) );
	Canny( blur, canny_output, 100, 200, 3 );
	return processLines(canny_output, threshold);
}

vector< pair< pair<int, int>, pair<int, int> > > MainLinesDetection::processLines(Mat data, int threshold) {
	vector< pair< pair<int, int>, pair<int, int> > >lines;
	Size img_size(data.cols, data.rows);

	if(threshold == 0)
		threshold = img_size.width > img_size.height ? img_size.width/4 : img_size.height/4;

	double hough_h = ((sqrt(2.0) * (double)(img_size.height > img_size.width ? img_size.height : img_size.width)) / 2.0); // finding out max rho
	Size acum_size(180, hough_h * 2.0); // -rho -> +rho, 0 -> 180

	double center_x = img_size.width/2;
	double center_y = img_size.height/2;

	int* acum = (int*)calloc(acum_size.width * acum_size.height, sizeof(int));

	for(int y=0; y < img_size.height; y++) {
		for(int x=0 ; x < img_size.width ; x++) {
			if( data.data[ (y * img_size.width) + x] > 250 ) {
				for(int t=0;t<180;t++) {
					double r = ( ((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));
					acum[(int)(round(r + hough_h) * 180.0) + t] ++;
				}
			}
		}
	}

	int max_v = 0, max_r = 0, max_t = 0;
	for(int r=0;r< acum_size.height;r++)
	{
		for(int t=0;t< acum_size.width;t++)
		{
			if((int)acum[(r*acum_size.width) + t] > max_v) {
				max_v = (int)acum[(r*acum_size.width) + t];
				max_t = t;
				max_r = r;
			}
			if((int)acum[(r*acum_size.width) + t] >= threshold)
			{
				//Is this point a local maxima (9x9)
				int max = acum[(r*acum_size.width) + t];
				for(int ly=-4;ly<=4;ly++)
				{
					for(int lx=-4;lx<=4;lx++)
					{
						if( (ly+r>=0 && ly+r<acum_size.height) && (lx+t>=0 && lx+t<acum_size.width)  )
						{
							if( (int)acum[( (r+ly)*acum_size.width) + (t+lx)] > max )
							{
								max = acum[( (r+ly)*acum_size.width) + (t+lx)];
								ly = lx = 5;
							}
						}
					}
				}
				if(max > (int)acum[(r*acum_size.width) + t])
					continue;


				int x1, y1, x2, y2;
				x1 = y1 = x2 = y2 = 0;

				if(t >= 45 && t <= 135)
				{
					//y = (r - x cos(t)) / sin(t)
					x1 = 0;
					y1 = ((double)(r-(acum_size.height/2)) - ((x1 - (img_size.width /2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (img_size.height / 2);
					x2 = img_size.width - 0;
					y2 = ((double)(r-(acum_size.height/2)) - ((x2 - (img_size.width /2) ) * cos(t * DEG2RAD))) / sin(t * DEG2RAD) + (img_size.height / 2);
				}
				else
				{
					//x = (r - y sin(t)) / cos(t);
					y1 = 0;
					x1 = ((double)(r-(acum_size.height/2)) - ((y1 - (img_size.height/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (img_size.width / 2);
					y2 = img_size.height - 0;
					x2 = ((double)(r-(acum_size.height/2)) - ((y2 - (img_size.height/2) ) * sin(t * DEG2RAD))) / cos(t * DEG2RAD) + (img_size.width / 2);
				}

				lines.push_back(std::pair< std::pair<int, int>, std::pair<int, int> >(std::pair<int, int>(x1,y1), std::pair<int, int>(x2,y2)));
				if(lines.size() == MAX_LINES) {
					free(acum);
					return lines;
				}
			}
		}

	}
	free(acum);
	return lines;
}
