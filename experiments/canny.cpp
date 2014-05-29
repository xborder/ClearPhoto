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
#include <cmath>

#define DEG2RAD 0.017453293f
#define SKY_THRESHOLD 60 // sky probability over 60%

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
int median_rho = 0;
int median_theta = 0;
/// Function header
void applyHough(Mat, int*, double (*)(int,int,double,double));
void horizonEdgeDetection(int*, double (*)(int,int,double,double));
void horizonColorDetection(int*, double (*)(int,int,double,double));
double intensifyFunction(int, int, double, double);
double deviation(vector<int> v, double ave);


/**
 * @function main
 */
int main( int, char** argv )
{
  Mat file = imread( argv[1], 1 );
  /// Load source image and convert it to gray
  resize(file, src, Size(), 0.5,0.5, INTER_NEAREST);

  int* color_output = (int*)calloc(6, sizeof(int));
  int* edge_output = (int*)calloc(6, sizeof(int));

//  horizonColorDetection(color_output, NULL);
//  median_theta = color_output[4];
//  median_rho = color_output[5];

  //printf("median_theta: %d median_rho: %d\n", median_theta, median_rho);
  int* cenas = (int*)calloc(6*3, sizeof(int));
  horizonEdgeDetection(cenas, NULL);

  //horizonEdgeDetection(edge_output, intensifyFunction);


  //line(src, Point(color_output[0], color_output[1]), Point(color_output[2], color_output[3]), Scalar(0,0,155), 1, CV_AA);
  //line(src, Point(edge_output[0], edge_output[1]), Point(edge_output[2], edge_output[3]), Scalar(0,155,0), 1, CV_AA);
  
  line(src, Point(cenas[0], cenas[1]), Point(cenas[2], cenas[3]), Scalar(155,0,0), 1, CV_AA);
  line(src, Point(cenas[6], cenas[7]), Point(cenas[8], cenas[9]), Scalar(155,0,0), 1, CV_AA);
  line(src, Point(cenas[12], cenas[13]), Point(cenas[14], cenas[15]), Scalar(155,0,0), 1, CV_AA);

  const char* source_window = "Source";
  namedWindow( source_window, WINDOW_AUTOSIZE );
  imshow( source_window, src );
  
  waitKey(0);
  return(0);
}

void horizonEdgeDetection(int* output, double (*intensifyFunction)(int,int,double,double)) {
  Mat yuv;
  cvtColor( src, yuv, COLOR_BGR2YUV );

  vector<Mat> yuv_planes;
  split( src, yuv_planes );

  Mat luma;
  luma = yuv_planes[0];

  /// Detect edges using canny
  Mat canny_output, blur;

  cv::blur( luma, blur, cv::Size(5,5) );
  Canny( blur, canny_output, thresh, thresh*2, 3 );

  // Closing operation, quanto maior o kernel, mais dispersos ficam os horizontes
  //Mat closing_output;
  //Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
  //morphologyEx(canny_output, closing_output, MORPH_CLOSE, kernel);

  //Hough transformation
  applyHough(canny_output, output, intensifyFunction);
}

void horizonColorDetection(int* output, double (*intensifyFunction)(int,int,double,double)) {
  Mat yuv;
  cvtColor( src, yuv, COLOR_BGR2YUV );

  vector<Mat> yuv_planes;
  split( src, yuv_planes );

  Mat grass = Mat::zeros(src.rows, src.cols, CV_8UC1);
  printf("Y: %dx%d U:%dx%d V:%dx%d\n", yuv_planes[0].cols, yuv_planes[0].rows,yuv_planes[1].cols, yuv_planes[1].rows,yuv_planes[2].cols, yuv_planes[2].rows);
  int YM = 210, UM = 150, VM = 100;
  int VARY = 130, VARU = 40, VARV = 40; 
  int total = src.rows*src.cols;
  for(int i = 0;i < src.rows;i++){
    double P = pow((double)i / src.rows, 2);
    double positionP = exp(-P);
    for(int j = 0;j < src.cols;j++){
//      uchar y = src.data[i * src.cols + j];
//      uchar u = src.data[(i / 2) * (src.cols / 2) + (j / 2) + total];
//      uchar v = src.data[(i / 2) * (src.cols / 2) + (j / 2) + total + (total/ 4)];
      uchar y = yuv_planes[0].at<uchar>(i,j);
      uchar u = yuv_planes[1].at<uchar>(i,j);
      uchar v = yuv_planes[2].at<uchar>(i,j);
      double A = (y-YM)/VARY;
      double B = (u-UM)/VARU;
      double C = (v-VM)/VARV;
      double Z = (pow(A,2)+ pow(B,2) + pow(C,2));
      double colorP = exp(-Z);

      int t = 0;
      for(int k = -2; k < 2; k++) {
        if(j-k < 0 || j+k >= src.rows)
          continue;
        t += abs(y - yuv_planes[0].at<uchar>(i,j + k));
      }

      int t0 = 10;
      double textureP = (t > t0) ? exp(-0.2*pow(t - t0,2)) : 1;

      double Psky = (positionP*colorP*textureP) ;
      grass.at<uchar>(i,j) = (Psky * 100 > SKY_THRESHOLD) ? 255 : 0;
    }
  }

  Mat canny_output;
  Canny(grass, canny_output, thresh, thresh*2, 3 );

  applyHough(canny_output, output, intensifyFunction);

  const char* source_window = "grass";
  namedWindow( source_window, WINDOW_AUTOSIZE );
  imshow( source_window, grass);
}

double intensifyFunction(int rho, int theta, double rho_deviation, double theta_deviation) {

  //printf("%f \n",exp(-(pow(rho-median_rho,2)/2 + pow(theta - median_theta,2)/2)));
  return exp(-(pow(rho-median_rho,2)/(2*pow(theta_deviation,2)) + pow(theta - median_theta,2)/(2*pow(theta_deviation,2))));//exp(-(pow(rho-median_rho,2)/2 + pow(theta - median_theta,2)/2));
}

double deviation(vector<int> v, double ave) {
       double E=0;
       int size = v.size();
       for(int i=0;i<size;i++)
               E+=(v[i] - ave)*(v[i] - ave);
       return sqrt(1/(double)size*E);
}

void applyHough(Mat binary_image, int* output, double (*func)(int,int,double,double)) {
  Size img_size(binary_image.cols, binary_image.rows);

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
       if( binary_image.data[ (y * img_size.width) + x] > 250 ) {
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

  if(func != NULL) {
    int rhos_avg = rho_sum/count;
    int thetas_avg = theta_sum/count;

    rho_deviation = deviation(rhos, rhos_avg);
    theta_deviation = deviation(thetas, thetas_avg);

    //printf("median_rho: %d rho_deviation: %f rho_avg: %d median_theta: %d theta_deviation: %f thetas_avg: %d\n", median_rho, rho_deviation, rhos_avg, median_theta, theta_deviation, thetas_avg);
    //for(int r=0; r < acum_size.height ; r++) {
    //  for(int t=0; t < acum_size.width;t++) {
    //    acum[(r*acum_size.width) + t] *= func(r,t,rho_deviation,theta_deviation);
          //printf("%f\n",exp(-(pow(r-median_rho,2)/(2*pow(rho_deviation,2)) + pow(t - median_theta,2)/(2*pow(theta_deviation,2)))));
          //printf("rho: %3d theta: %3d val: %4f* %f -> %f\n", r, t, acum[(r*acum_size.width) + t], exp(-(pow(r-median_rho,2)/(2*pow(theta_deviation,2)) + pow(t - median_theta,2)/(2*pow(theta_deviation,2)))), acum[(r*acum_size.width) + t]*exp(-(pow(r-median_rho,2)/(2*pow(theta_deviation,2)) + pow(t - median_theta,2)/(2*pow(theta_deviation,2)))));
    //  }
    //}
  }
  int i = 0, offset = 0, last_max = INT_MAX, last_rho = INT_MAX, last_theta = INT_MAX;
  while(i < 3) {
    int max_val = 0, max_rho = 0, max_theta = 0;
    for(int r=0; r < acum_size.height ; r++) {
      for(int t=0; t < acum_size.width; t++) {
        double factor = (func != NULL) ? func(r,t,rho_deviation,theta_deviation) : 1;
        if(acum[(r*acum_size.width) + t] * factor > max_val && acum[(r*acum_size.width) + t] * factor < last_max && r != last_rho && t != last_theta) {
          max_rho = r;
          max_theta = t;
          max_val = acum[(r*acum_size.width) + t];
        }
      }
    }

    if(max_rho == -1 && max_theta == -1) {
      output[0] = output[1] = output[2] = output[3] = output[4] = output[5] = -1;
      i++;
      continue;
//      free(acum);
//      return;
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
    output[offset++] = x1;
    output[offset++] = y1;
    output[offset++] = x2;
    output[offset++] = y2;
    output[offset++] = t;
    output[offset++] = r;
    
    last_max = max_val;
    last_rho = max_rho;
    last_theta = max_theta;
    i++;
    printf("x1: %d y1: %d x2: %d y2: %d max val: %d max rho: %d max theta: %d \n", x1,y1,x2,y2, max_val, max_rho, max_theta);
  //line(src, Point(x1, y1), Point(x2, y2), Scalar(0,0,255), 1, CV_AA);
  }
}