/**
 * @function findContours_Demo.cpp
 * @brief Demo code to find contours in an image
 * @author OpenCV team
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <cfloat>

#define DEG2RAD 0.017453293f

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;

/// Function header
void thresh_callback(int, void* );
void method1();
void method2();
void applyHough(Mat);

/**
 * @function main
 */
int main( int, char** argv )
{
  Mat file = imread( argv[1], 1 );
  /// Load source image and convert it to gray
  resize(file, src, Size(), 0.5,0.5, INTER_NEAREST);

  /// Create Window
  const char* source_window = "Source";
  namedWindow( source_window, WINDOW_AUTOSIZE );
  imshow( source_window, src );

/*  method1();*/
  method2();

  waitKey(0);
  return(0);
}

/*void method1() {
  vector<Mat> bgr_planes;
  split( src, bgr_planes );

  Mat blueChannel;
  blueChannel = bgr_planes[0];

  /// Detect edges using canny
  Mat output;
  Canny( blueChannel, output, thresh, thresh*2, 3 );

  const char* window_name = "method1";
  namedWindow( window_name, WINDOW_AUTOSIZE );
  imshow( window_name, blueChannel );
}*/

void method2() {
  Mat yuv;
  cvtColor( src, yuv, COLOR_BGR2YUV );

  vector<Mat> yuv_planes;
  split( src, yuv_planes );

  Mat luma;
  luma = yuv_planes[0];

  /// Detect edges using canny
  Mat canny_output;
  Canny( luma, canny_output, thresh, thresh*2, 3 );

  // Closing operation, quanto maior o kernel, mais dispersos ficam os horizontes
  Mat closing_output;
  Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
  morphologyEx(canny_output, closing_output, MORPH_CLOSE, kernel);

  //Hough transformation
  applyHough(closing_output);
  
  const char* window_name = "method2";
  namedWindow( window_name, WINDOW_AUTOSIZE );
  imshow( window_name, luma );

  const char* canny_window_name = "canny output";
  namedWindow( canny_window_name, WINDOW_AUTOSIZE );
  imshow( canny_window_name, canny_output);

  const char* closing_window_name = "closing output";
  namedWindow( closing_window_name, WINDOW_AUTOSIZE );
  imshow( closing_window_name, closing_output);

  const char* hough_window_name = "hough transform output";
  namedWindow( hough_window_name, WINDOW_AUTOSIZE );
  imshow( hough_window_name, src);
}

void applyHough(Mat binary_image) {
  Size img_size(binary_image.cols, binary_image.rows);

  double hough_h = ((sqrt(2.0) * (double)(img_size.height > img_size.width ? img_size.height : img_size.width)) / 2.0); // finding out max rho
  Size acum_size(180, hough_h * 2.0); // -rho -> +rho, 0 -> 180
  
  double center_x = img_size.width/2;
  double center_y = img_size.height/2;
  int threshold = img_size.width>img_size.height?img_size.width/4:img_size.height/4;

  int* acum = (int*)calloc(acum_size.width * acum_size.height, sizeof(int));
  for(int y=0; y < img_size.height ;y++)
   {
     for(int x=0 ; x < img_size.width ; x++)
     {
       if( binary_image.data[ (y * img_size.width) + x] > 250 )
       {
         for(int t=0;t<180;t++)
         {
            double r = ( ((double)x - center_x) * cos((double)t * DEG2RAD)) + (((double)y - center_y) * sin((double)t * DEG2RAD));
            acum[(int)((round(r + hough_h) * 180.0)) + t] ++;
         }
       }
     }
  }

  int max_val = 0, max_rho = 0, max_theta = 0;
  for(int r=0; r < acum_size.height ; r++) {
    for(int t=0; t < acum_size.width;t++) {
      if((int)acum[(r*acum_size.width) + t] > max_val) {
        max_rho = r;
        max_theta = t;
        max_val = (int)acum[(r*acum_size.width) + t];
      }
    }
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
  printf("x1: %d y1: %d x2: %d y2: %d\n", x1,y1,x2,y2);
  line( src, Point(x1, y1), Point(x2, y2), Scalar(0,0,255), 1, CV_AA);

  //get intensity for each pixel
/*  for(int x = 0; x < img_size.width; x++) {
    for(int y = 0; y < img_size.height; y++) {
      unsigned char pixel = binary_image.at<unsigned char>(x,y);
      if(pixel > 250) {
        for(int theta = 0; theta < 180; theta++) {
          double rho = ( ((double)x - center_x) * cos((double)theta * DEG2RAD)) + (((double)y - center_y) * sin((double)theta * DEG2RAD));
          acum[ (int)((round(rho + hough_h) * 180.0)) + theta]++;
        }
      }
    }
  }

  //get maximum intensity
  int max_acum = 0, max_rho = 0, max_theta = 0;
  for(int theta = 0; theta < acum_size.width; theta++) {
    for (int rho = 0; rho < acum_size.height; rho++) {
      int cur_acum = acum[(rho * acum_size.width) + theta];
      if(cur_acum > max_acum) {
        max_acum = cur_acum;
        max_rho = rho;
        max_theta = theta;
        /*printf("max_acum: %d, max_rho: %d, max_theta: %d\n", max_acum, max_rho, max_theta);
      }
    }
  }
 int px = max_rho * cos(max_theta);
  int py = max_rho * sin(max_theta);
  int maxLength = sqrt(img_size.width*img_size.width + img_size.height*img_size.height);
  int x1, y1, x2, y2;
  x1 = y1 = x2 = y2 = 0;

  if(max_theta >= 45 && max_theta <= 135) {
    //y = (r - x cos(t)) / sin(t)
    x1 = 0;
    y1 = ((double)(max_rho-(acum_size.height/2)) - ((x1 - (img_size.height /2) ) * cos(max_theta * DEG2RAD))) / sin(max_theta * DEG2RAD) + (img_size.height / 2);
    x2 = img_size.width - 0;
    y2 = ((double)(max_rho-(acum_size.height/2)) - ((x2 - (img_size.width /2) ) * cos(max_theta * DEG2RAD))) / sin(max_theta * DEG2RAD) + (img_size.height / 2);
  } else {
    //x = (r - y sin(t)) / cos(t);
    y1 = 0;
    x1 = ((double)(max_rho-(acum_size.height/2)) - ((y1 - (img_size.height /2) ) * sin(max_theta * DEG2RAD))) / cos(max_theta * DEG2RAD) + (img_size.width / 2);
    y2 = img_size.height - 0;
    x2 = ((double)(max_rho-(acum_size.height/2)) - ((y2 - (img_size.height /2) ) * sin(max_theta * DEG2RAD))) / cos(max_theta * DEG2RAD) + (img_size.width / 2);
  }

  p1_x = px + maxLength * cos(max_theta);
  p1_y = py + maxLength * sin(max_theta);
  p2_x = px - maxLength * cos(max_theta);
  p2_y = py - maxLength * sin(max_theta);

  line( src, Point(x1, y1), Point(x2, y2), Scalar(0,0,255), 1, CV_AA);*/
}