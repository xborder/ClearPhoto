//
//  main.cpp
//  FSD
//
//  Created by Saburo Okita on 02/05/14.
//  Copyright (c) 2014 Saburo Okita. All rights reserved.
//

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

#include <tbb/tbb.h>
#include "FastSymmetryDetector.h"

#include "ObjectSegmentation.h"


using namespace std;
using namespace cv;

string filename = "/home/helder/flickr/symmetry/14392340540.jpg";

static Point accumIndex(-1, -1);
static void onMouse( int event, int x, int y, int, void * data );

int clusterCount = 2;

vector<Point> centers;
map<int,int> countClusters(Mat& image, Mat& mask);
void getBinaryImage(const Mat& saliency, Mat& binary, Point& center, Rect& rect);
void getSegmentationMask(const Mat& image, Mat& mask);
int getCenterMass(const Mat& img);
vector<int> divideRegions(Mat& image);
void putTextCount(int x, int y, Mat& img, int val);
void sideWeight(Mat& image, Mat& canny, pair<Point, Point> symmetryLine);
void getDivisoryMask(const Mat& image, pair<Point,Point> symmetryLine, Mat& leftMask, Mat& rightMask, int& leftCount, int& rightCount);
void countNonZeros(const Mat& canny, const Mat& mask);
vector<Point> generateClusters(Mat& image, Mat& clustered);

void testImage();
void testVideo();

int main(int argc, const char * argv[]) {
    filename = argv[1];
    testImage();
//    testVideo();
    
    return 0;
}

/**
 * Test using the scene captured from the webcam video
 */
void testVideo() {
    namedWindow("");
    moveWindow("", 0, 0);
    
    Mat frame;
    VideoCapture cap(0);
    while( frame.empty() )
        cap >> frame;
    
    /* Resize the image accordingly */
    resize( frame, frame, Size(), 0.7, 0.7 );
    
    /* Determine the shape of Hough accumulationmatrix */
    float rho_divs   = hypotf( frame.rows, frame.cols ) + 1;
    float theta_divs = 180.0;
    
    FastSymmetryDetector detector( frame.size(), Size(rho_divs, theta_divs), 1 );
    
    /* Adjustable parameters, depending on the scene condition */
    int canny_thresh_1 = 30;
    int canny_thresh_2 = 90;
    int min_pair_dist  = 25;
    int max_pair_dist  = 500;
    int no_of_peaks    = 5;
    
    createTrackbar( "canny_thresh_1", "", &canny_thresh_1, 500 );
    createTrackbar( "canny_thresh_2", "", &canny_thresh_2, 500 );
    createTrackbar( "min_pair_dist", "", &min_pair_dist, 500 );
    createTrackbar( "max_pair_dist", "", &max_pair_dist, 500 );
    createTrackbar( "no_of_peaks", "", &no_of_peaks, 10 );
    
    Mat edge;
    while( true ){
        cap >> frame;
        flip( frame, frame, 1 );
        resize( frame, frame, Size(), 0.7, 0.7 );
        
        /* Find the edges of the image */
        cvtColor( frame, edge, CV_BGR2GRAY );
        Canny( edge, edge, canny_thresh_1, canny_thresh_2 );
        
        /* Vote for the hough matrix */
        detector.vote( edge, min_pair_dist, max_pair_dist );
        Mat accum = detector.getAccumulationMatrix();
        
        /* Get the result and draw the symmetrical line */
        vector<pair<Point, Point> > result = detector.getResult( no_of_peaks );
        Point mean1, mean2;
        for( auto point_pair: result ) {
            line(frame, point_pair.first, point_pair.second, Scalar(0, 0, 255), 3);
            mean1.x += point_pair.first.x;
            mean1.y += point_pair.first.y;
            mean2.x += point_pair.second.x;
            mean2.y += point_pair.second.y;
        }
        mean1.x /= no_of_peaks;
        mean1.y /= no_of_peaks;
        mean2.x /= no_of_peaks;
        mean2.y /= no_of_peaks;
        line(frame, mean1, mean2, Scalar(0, 255, 255), 3);
        
        /* Convert our Hough accum matrix to heat map */
        accum.convertTo( accum, CV_8UC3 );
        applyColorMap( accum, accum, COLORMAP_JET );
        resize( accum, accum, Size(), 2.0, 0.5 );
        
        /* Show the original, edge and the accumulation image */
        Mat appended = Mat::zeros( frame.rows + accum.rows, frame.cols * 2, CV_8UC3 );
        frame.copyTo( Mat(appended, Rect(0, 0, frame.cols, frame.rows)) );
        cvtColor( edge, Mat(appended, Rect(frame.cols, 0, edge.cols, edge.rows)), CV_GRAY2BGR );
        accum.copyTo( Mat( appended, Rect(0, frame.rows, accum.cols, accum.rows) ) );

        
        imshow( "", appended );
        if(waitKey(10) == 'q')
            break;
    }
}


/*
 * Test on given image
 */
void testImage() {
    namedWindow("");
    moveWindow("", 0, 0);
    
    Mat frame = imread( filename.c_str(), 1);
    /* Determine the shape of Hough accumulationmatrix */
    float rho_divs   = hypotf( frame.rows, frame.cols ) + 1;
    float theta_divs = 180.0;
    
    FastSymmetryDetector detector( frame.size(), Size(rho_divs, theta_divs), 1 );
    
    
    Rect region( 0, frame.rows, theta_divs * 2.0, rho_divs * 0.5 );
    setMouseCallback( "", onMouse, static_cast<void*>( &region ) );
    Mat temp, edge;
    
    /* Adjustable parameters, depending on the scene condition */
    int canny_thresh_1 = 30;
    int canny_thresh_2 = 90;
    int min_pair_dist  = 25;
    int max_pair_dist  = 500;
    int no_of_peaks    = 3;
    
    createTrackbar( "canny_thresh_1", "", &canny_thresh_1, 500 );
    createTrackbar( "canny_thresh_2", "", &canny_thresh_2, 500 );
    createTrackbar( "min_pair_dist", "", &min_pair_dist, 500 );
    createTrackbar( "max_pair_dist", "", &max_pair_dist, 500 );
    createTrackbar( "no_of_peaks", "", &no_of_peaks, 10 );
    
    while( true ) {
        temp = frame.clone();
        
        /* Find the edges */
        cvtColor( temp, edge, CV_BGR2GRAY );
        Canny( edge, edge, canny_thresh_1, canny_thresh_2 );
        
        /* Vote for the accumulation matrix */
        detector.vote( edge, min_pair_dist, max_pair_dist );
        
        /* Draw the symmetrical line */
        vector<pair<Point, Point> > result = detector.getResult( no_of_peaks );
        Point mean1, mean2;
        int i = 0;
        for( auto point_pair: result ) {
            if (i == 0) {
                line(temp, point_pair.first, point_pair.second, Scalar(255, 0, 0), 3);
                i++;
            } else {
                line(temp, point_pair.first, point_pair.second, Scalar(0, 0, 255), 3);
            }
            mean1.x += point_pair.first.x;
            mean1.y += point_pair.first.y;
            mean2.x += point_pair.second.x;
            mean2.y += point_pair.second.y;
        }
        mean1.x /= no_of_peaks;
        mean1.y /= no_of_peaks;
        mean2.x /= no_of_peaks;
        mean2.y /= no_of_peaks;
        line(temp, mean1, mean2, Scalar(0, 255, 255), 3);
        /* Visualize the Hough accum matrix */
        Mat accum = detector.getAccumulationMatrix();
        accum.convertTo( accum, CV_8UC3 );
        applyColorMap( accum, accum, COLORMAP_JET );
        resize( accum, accum, Size(), 2.0, 0.5 );
        
        /* Draw lines based on cursor position */
        if(accumIndex.x != -1 && accumIndex.y != -1 ) {
            pair<Point, Point> point_pair = detector.getLine( accumIndex.y, accumIndex.x );
            line( temp, point_pair.first, point_pair.second, CV_RGB(0, 255, 0), 2 );
        }
        
        /* Show the original and edge images */
        Mat appended = Mat::zeros( temp.rows + accum.rows, temp.cols * 2, CV_8UC3 );
        temp.copyTo( Mat(appended, Rect(0, 0, temp.cols, temp.rows)) );
        cvtColor( edge, Mat(appended, Rect(temp.cols, 0, edge.cols, edge.rows)), CV_GRAY2BGR );
        accum.copyTo( Mat( appended, Rect(0, temp.rows, accum.cols, accum.rows) ) );
        
        imshow( "", appended );
        if(waitKey(10) == 'q')
            break;
//        if(waitKey(33) == 'a') {
            sideWeight(frame, edge, pair<Point,Point>(mean1, mean2));
//        }
    }
}


void sideWeight(Mat& image, Mat& canny, pair<Point, Point> symmetryLine) {
  Mat mask;
  getSegmentationMask(image, mask);
  imshow("segment", mask);
  waitKey(0);
  Mat leftMask, rightMask;
  int leftCount = 0, rightCount = 0;
  getDivisoryMask(image, symmetryLine, leftMask, rightMask, leftCount, rightCount);

  Mat gray;
  cvtColor(image, gray, CV_BGR2GRAY);
  Mat tmp;
  cvtColor(image, tmp, CV_BGR2HSV);

  vector<Mat> hsv;
  split(tmp,hsv);

  Mat clustered(image.rows, image.cols, CV_32FC1);
  centers = generateClusters(gray, clustered);
  clustered.convertTo(clustered, CV_8U);

  map<int,int> leftClusterCount = countClusters(clustered, leftMask);
  map<int,int> rightClusterCount = countClusters(clustered, rightMask);

  printf("Total: %d\t\t LEFT - cluster 1: %d (%f%%) | cluster 2: %d (%f%%)\n", leftCount, leftClusterCount.at(0), leftClusterCount.at(0)/(float)leftCount*100, leftClusterCount.at(1), leftClusterCount.at(1)/(float)leftCount*100);
  printf("Total: %d\t\t RIGHT - cluster 1: %d (%f%%) | cluster 2: %d (%f%%)\n", rightCount, rightClusterCount.at(0), rightClusterCount.at(0)/(float)rightCount*100, rightClusterCount.at(1), rightClusterCount.at(1)/(float)rightCount*100);

  imshow("clustered", clustered);
  imshow("gray", gray);
}

map<int,int> countClusters(Mat& image, Mat& mask) {
  map<int,int> counter;
  for(int i = 0 ; i < clusterCount; i++) {
    counter.insert ( pair<int,int>(i,0) );
  }
  //Cluster 1;
  for(int x = 0; x < image.cols; x++){
    for(int y = 0; y < image.rows; y++){
      for(int j=0; j < clusterCount; j++) {
        if(image.at<uchar>(y,x) == 255/(j+1) && mask.at<uchar>(y,x) == 255) {
          counter.at(j)++;
          break;
        }
      }
    }
  }
  return counter;
}

vector<Point> generateClusters(Mat& image, Mat& clustered) {
  RNG rng(12345);
  //Collecting samples for kmeans;
  int subRegionWidth = image.cols;
  int subRegionHeight = image.rows;

  Mat samples(subRegionHeight * subRegionWidth, 1,  CV_32F);

  for(int i=0; i < subRegionHeight*subRegionWidth; i++) {
    samples.at<float>(i,0) = image.data[i];
  }

  Mat labels;
  int attempts = 5;
  Mat cent;
  kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, cent );

  cout << cent << endl;
  int colors[clusterCount];
  for(int i=0; i<clusterCount; i++) {
    colors[i] = 255/(i+1);
  }
    // i think there is a better way to do this mayebe some Mat::reshape?
  vector<Point> toReturn;
  map<int,Point> cluster_centers;
  map<int,int> average_count;
  for(int i = 0 ; i < clusterCount; i++) {
    cluster_centers.insert ( pair<int,Point>(i,Point(0,0)) );
    average_count.insert ( pair<int,int>(i,0) );
  }

  for(int i = 0; i < subRegionWidth * subRegionHeight; i++) {
    int y = i / subRegionWidth;
    int x = i % subRegionWidth; 
    cluster_centers.at(labels.at<int>(0,i)).x += x;
    cluster_centers.at(labels.at<int>(0,i)).y += y;
    average_count.at(labels.at<int>(0,i))++;
    clustered.at<float>(y,x) = (float)(colors[labels.at<int>(0,i)]);
  }


  for(int i = 0 ; i < cluster_centers.size(); i++) {
    Point point = cluster_centers.at(i);
    int count = average_count.at(i);
    point.x /= count;
    point.y /= count;
    cluster_centers.at(i) = point;
    toReturn.push_back(point);
    cout << average_count.at(i) << endl;
    circle(image, Point(point.x,point.y), 2, Scalar(0,0,0),5);
  }

  int sum = 0;
  sum += sqrt(pow(cluster_centers[0].x - cluster_centers[1].x, 2) + pow(cluster_centers[0].y - cluster_centers[1].y, 2));
  sum += sqrt(pow(cluster_centers[1].x - cluster_centers[2].x, 2) + pow(cluster_centers[1].y - cluster_centers[2].y, 2));
  sum += sqrt(pow(cluster_centers[2].x - cluster_centers[0].x, 2) + pow(cluster_centers[2].y - cluster_centers[0].y, 2));

  ostringstream sb;
  sb << sum;
  string s =  sb.str();
  putText(image, s.c_str(), Point(20,20), FONT_HERSHEY_DUPLEX, 0.5, Scalar::all(0));
  return toReturn;
}

void countNonZeros(const Mat& canny, const Mat& mask) {
    Mat tmp = Mat::zeros(canny.size(), CV_8UC1);
    bitwise_and(canny, mask, tmp);
    int nonZeros = countNonZero(tmp);
    float perc = nonZeros/(float)countNonZero(canny)*100;
    cout << nonZeros << "(" << perc << "%)" << endl;
}

void getDivisoryMask(const Mat& image, pair<Point,Point> symmetryLine, Mat& leftMask, Mat& rightMask, int& leftCount, int& rightCount) {
    leftMask = Mat::zeros(image.rows, image.cols, CV_8UC1);
    rightMask = Mat::zeros(image.rows, image.cols, CV_8UC1);
    Point p1 = symmetryLine.first;
    Point p2 = symmetryLine.second;

    if((p2.x - p1.x) == 0) { // é recta vertical
        unsigned char *leftoutput= (unsigned char*)(leftMask.data);
        unsigned char *rightoutput= (unsigned char*)(rightMask.data);
        for(int x = 0; x < image.cols; x++){
          for(int y = 0; y < image.rows; y++){
            if(x < p1.x) { // isLeft
                leftCount++;
                leftoutput[image.cols * y + x] = 255;
            } else { //isRight
                rightCount++;
                rightoutput[image.cols * y + x] = 255;
            }
          }
        }
        /*
        cout << "left: " << leftCount.at(0) << endl;
        cout << "right: " << rightCount.at(0) << endl;
        imshow("left division", leftMask);
        imshow("right division", rightMask);
        waitKey(0);*/
        return ;
    } 

  double m = -1/(double)((p2.y - p1.y) / (p2.x - p1.x)); 
  if ( m == 0.0) { // é recta horizontal
    unsigned char *leftoutput= (unsigned char*)(leftMask.data);
    unsigned char *rightoutput= (unsigned char*)(rightMask.data);
    for(int x = 0; x < image.cols; x++){
      for(int y = 0; y < image.rows; y++){
        if(y < p1.y) { // isLeft
            leftCount++;
            leftoutput[image.cols * y + x] = 255;
        } else { //isRight
            rightCount++;
            rightoutput[image.cols * y + x] = 255;
        }
      }
    }
  } else {
    int y0 = p1.y;
    int x0 = p1.x;

    int y1 = p2.y;
    int x1 = p2.x;

    unsigned char *leftoutput= (unsigned char*)(leftMask.data);
    unsigned char *rightoutput= (unsigned char*)(rightMask.data);
    for(int x = 0; x < image.cols; x++){
        for(int y = 0; y < image.rows; y++){
            if((x0 - x1)*(y - y1) < (y0 - y1)*(x - x1)) { // isRight
                rightCount++;
                rightoutput[image.cols * y + x] = 255;
            } else { //isLeft
                leftCount++;
                leftoutput[image.cols * y + x] = 255;
            }
        }
    }
  }
  /*
  cout << "left: " << leftCount.at(0) << endl;
  cout << "right: " << rightCount.at(0) << endl;
  imshow("left division", leftMask);
  imshow("right division", rightMask);
  waitKey(0);*/
}

/**
 * Mouse callback, to show the line based on which part of accumulation matrix the cursor is.
 */
static void onMouse( int event, int x, int y, int, void * data ) {
    Rect *region = (Rect*) data;
    Point point( x, y );
    
    if( (*region).contains( point ) ) {
        accumIndex.x = (point.x - region->x) / 2.0;
        accumIndex.y = (point.y - region->y) * 2.0;
    }
    else {
        accumIndex.x = -1;
        accumIndex.y = -1;
    }
}

int getCenterMass(const Mat& img) {
  int x = 0, y = 0, count = 0;
  for (int r = 0; r < img.rows; r++) {
    const uchar* binary_row = img.ptr<uchar>(r);
    for (int c = 0; c < img.cols; c++) {
      if(binary_row[c] > 20) {
        x += c;
        y += r;
        count++;
      }
    }
  }
  Point center;
  if(count > 0) 
    center = Point(x/count, y/count);
  else
    center = Point(0,0);
  return count;
}

vector<int> divideRegions(Mat& image) {
  int subRegionWidth = image.cols/3, subRegionHeight = image.rows/3;
  printf("%d %d\n", image.cols/3, image.rows/3);
  vector<int> centers;

  Rect topLeftRegion(Point(0,0), Point(subRegionWidth, subRegionHeight));
  Rect topMiddleRegion(Point(subRegionWidth, 0), Point(2*subRegionWidth, subRegionHeight));
  Rect topRightRegion(Point(2*subRegionWidth, 0), Point(3*subRegionWidth, subRegionHeight));

  Rect midLeftRegion(Point(0, subRegionHeight), Point(subRegionWidth, 2*subRegionHeight));
  Rect midMiddleRegion(Point(subRegionWidth, subRegionHeight), Point(2*subRegionWidth, 2*subRegionHeight));
  Rect midRightRegion(Point(2*subRegionWidth, subRegionHeight), Point(3*subRegionWidth, 2*subRegionHeight));

  Rect bottomLeftRegion(Point(0, 2*subRegionHeight), Point(subRegionWidth, 3*subRegionHeight));
  Rect bottomMiddleRegion(Point(subRegionWidth, 2*subRegionHeight), Point(2*subRegionWidth, 3*subRegionHeight));
  Rect bottomRightRegion(Point(2*subRegionWidth, 2*subRegionHeight), Point(3*subRegionWidth, 3*subRegionHeight));

  int center1 = getCenterMass(image(topLeftRegion));
  int center2 = getCenterMass(image(topMiddleRegion));
  int center3 = getCenterMass(image(topRightRegion));
  
  int center4 = getCenterMass(image(midLeftRegion));
  int center5 = getCenterMass(image(midMiddleRegion));
  int center6 = getCenterMass(image(midRightRegion));

  int center7 = getCenterMass(image(bottomLeftRegion));
  int center8 = getCenterMass(image(bottomMiddleRegion));
  int center9 = getCenterMass(image(bottomRightRegion));

  centers.push_back(center1);
  centers.push_back(center2);
  centers.push_back(center3);
  centers.push_back(center4);
  centers.push_back(center5);
  centers.push_back(center6);
  centers.push_back(center7);
  centers.push_back(center8);
  centers.push_back(center9);

  rectangle(image, topLeftRegion, Scalar::all(255), 2);
  rectangle(image, topMiddleRegion, Scalar::all(255), 2);
  rectangle(image, topRightRegion, Scalar::all(255), 2);
  rectangle(image, midLeftRegion, Scalar::all(255), 2);
  rectangle(image, midMiddleRegion, Scalar::all(255), 2);
  rectangle(image, midRightRegion, Scalar::all(255), 2);
  rectangle(image, bottomLeftRegion, Scalar::all(255), 2);
  rectangle(image, bottomMiddleRegion, Scalar::all(255), 2);
  rectangle(image, bottomRightRegion, Scalar::all(255), 2);

  putTextCount(topLeftRegion.x, 50+topLeftRegion.y, image, center1);
  putTextCount(topMiddleRegion.x, 50+topMiddleRegion.y, image, center2);
  putTextCount(topRightRegion.x, 50+topRightRegion.y, image, center3);
  
  putTextCount(midLeftRegion.x, 50+midLeftRegion.y, image, center4);
  putTextCount(midMiddleRegion.x, 50+midMiddleRegion.y, image, center5);
  putTextCount(midRightRegion.x, 50+midRightRegion.y, image, center6);
  
  putTextCount(bottomLeftRegion.x, 50+bottomLeftRegion.y, image, center7);
  putTextCount(bottomMiddleRegion.x, 50+bottomMiddleRegion.y, image, center8);
  putTextCount(bottomRightRegion.x, 50+bottomRightRegion.y, image, center9);

  return centers;
}

void putTextCount(int x, int y, Mat& img, int val) {
  ostringstream sb;
  sb << val;
  string s =  sb.str();
  rectangle(img, Point(x,y), Point(x+50, y-20), Scalar::all(0), -1, 8);
  putText(img, s.c_str(), Point(x,y), FONT_HERSHEY_DUPLEX, 0.5, Scalar::all(255));
}


void getSegmentationMask(const Mat& image, Mat& mask) {
  mask = Mat::zeros(image.size(), CV_8UC1);
  Mat tmp_image;
  image.convertTo(tmp_image, CV_32FC3, 1.0/255);
  Mat hc = ObjectSegmentation::GetHC(tmp_image);
  Point center;
  Rect roi;
  Mat tmp_mask;
  getBinaryImage(hc*255, tmp_mask, center, roi);
  
  Mat pr_fgd, pr_bgd; 
  compare(tmp_mask, GC_PR_FGD, pr_fgd, CMP_EQ);
  compare(tmp_mask, GC_PR_BGD, pr_bgd, CMP_EQ);

  mask = pr_fgd;
/*
  Mat mask_rect = Mat::zeros(mask.size(), CV_8U);
  mask_rect(roi).setTo(255);
  Mat tmp;
  if(countNonZero(mask_rect) != 0) {
//    bitwise_or(pr_fgd, pr_bgd, mask);
    bitwise_and(pr_fgd, mask_rect, mask);
  } else {
    bitwise_or(pr_fgd, pr_bgd, mask);
//    bitwise_or(tmp, mask_rect, mask);
  }*/
  imshow("sda", pr_fgd);
  waitKey(0);
}

void getBinaryImage(const Mat& saliency, Mat& binary, Point& center, Rect& rect) {
  double minVal;
  double maxVal;
  Point minLoc;
  Point maxLoc;
  minMaxLoc(saliency, &minVal, &maxVal, &minLoc, &maxLoc );

  saliency.convertTo(binary, CV_8U, 255.0/(maxVal-minVal),-255.0*minVal/(maxVal-minVal));
  int center_x = 0, center_y = 0, count = 0;
  int first_x = 0, first_y = 0, last_x = 0, last_y = 0;
  int* rows = (int*)calloc(binary.rows, sizeof(int));
  int* cols = (int*)calloc(binary.cols, sizeof(int));

  for (int r = 0; r < binary.rows; r++) {
    uchar* binary_row = binary.ptr<uchar>(r);
    for (int c = 0; c < binary.cols; c++) {
      if(binary_row[c] >= MIN_FGD_THRESHOLD) {
        binary_row[c] = GC_PR_FGD;
        center_y += r;
        center_x += c;
        count++;
        rows[r]++;
        cols[c]++;
      } else if (binary_row[c] < MIN_FGD_THRESHOLD && binary_row[c] >= MIN_PR_BGD_THRESHOLD) {
        binary_row[c] = GC_PR_BGD;
      } else {
        binary_row[c] = GC_BGD;
      }
    }
  }
  center.x = center_x/count;
  center.y = center_y/count;
  int i = 0, blank_lines = 0;
  while(i < binary.cols && blank_lines < 50) {
    blank_lines++;
    if(center.x - i > 0  && cols[center.x - i] >= 25) {
      first_x = center.x - i;
      blank_lines = 0;
    }
    if(center.x + i < binary.cols && cols[center.x + i] >= 25) {
      last_x = center.x + i;
      blank_lines = 0;
    }
    i++;
  }
  i = 0, blank_lines = 0;
  while(i < binary.rows && blank_lines < 50) {
    blank_lines++;
    if(center.y - i > 0 && rows[center.y - i] >= 25) {
      first_y = center.y - i;
      blank_lines = 0;
    }
    if(center.y + i < binary.rows && rows[center.y + i] >= 25) {
      last_y = center.y + i;
      blank_lines = 0;
    }
    i++;
  }
  rect.x = first_x;
  rect.y = first_y;
  rect.width = last_x - first_x;
  rect.height = last_y - first_y;
  free(cols);
  free(rows);
}

Mat ObjectSegmentation::GetHC(const Mat &img3f)
{
  // Quantize colors and
  Mat idx1i, binColor3f, colorNums1i, weight1f, _colorSal;
  Quantize(img3f, idx1i, binColor3f, colorNums1i);


  cvtColor(binColor3f, binColor3f, CV_BGR2Lab);
  normalize(colorNums1i, weight1f, 1, 0, NORM_L1, CV_32F);
  GetHC(binColor3f, weight1f, _colorSal);


  float* colorSal = (float*)(_colorSal.data);
  Mat salHC1f(img3f.size(), CV_32F);
  for (int r = 0; r < img3f.rows; r++)
  {
    float* salV = salHC1f.ptr<float>(r);
    int* _idx = idx1i.ptr<int>(r);
    for (int c = 0; c < img3f.cols; c++){
      salV[c] = colorSal[_idx[c]];
    }
  }

  GaussianBlur(salHC1f, salHC1f, Size(3, 3), 0);
  normalize(salHC1f, salHC1f, 0, 1, NORM_MINMAX);
  return salHC1f;
}

void ObjectSegmentation::GetHC(const Mat &binColor3f, const Mat &weight1f, Mat &_colorSal)
{
  int binN = binColor3f.cols;
  _colorSal = Mat::zeros(1, binN, CV_32F);
  float* colorSal = (float*)(_colorSal.data);
  vector<vector<CostfIdx> > similar(binN); // Similar color: how similar and their index
  Vec3f* color = (Vec3f*)(binColor3f.data);
  float *w = (float*)(weight1f.data);
  for (int i = 0; i < binN; i++)
  {
    vector<CostfIdx> &similari = similar[i];
    similari.push_back(make_pair(0.f, i));
    for (int j = 0; j < binN; j++)
    {
      if (i == j)
        continue;
      float dij = vecDist3<float>(color[i], color[j]);
      similari.push_back(make_pair(dij, j));
      colorSal[i] += w[j] * dij;
    }
    sort(similari.begin(), similari.end());
  }

  SmoothSaliency(binColor3f, _colorSal, 4.0f, similar);
}

void ObjectSegmentation::SmoothSaliency(const Mat &binColor3f, Mat &sal1d, float delta, const vector<vector<CostfIdx> > &similar)
{
  if (sal1d.cols < 2)
    return;
  CV_Assert(binColor3f.size() == sal1d.size() && sal1d.rows == 1);
  int binN = binColor3f.cols;
  Vec3f* color = (Vec3f*)(binColor3f.data);
  Mat tmpSal;
  sal1d.copyTo(tmpSal);
  float *sal = (float*)(tmpSal.data);
  float *nSal = (float*)(sal1d.data);

  //* Distance based smooth
  int n = max(cvRound(binN/delta), 2);
  vecF dist(n, 0), val(n);
  for (int i = 0; i < binN; i++)
  {
    const vector<CostfIdx> &similari = similar[i];
    float totalDist = 0;

    val[0] = sal[i];
    for (int j = 1; j < n; j++)
    {
      int ithIdx =similari[j].second;
      dist[j] = similari[j].first;
      val[j] = sal[ithIdx];
      totalDist += dist[j];
    }
    float valCrnt = 0;
    for (int j = 0; j < n; j++)
      valCrnt += val[j] * (totalDist - dist[j]);
    nSal[i] =  valCrnt / ((n-1) * totalDist);
  }
  //*/

  /* Gaussian smooth
  const float guassCoeff = -0.5f/(delta*delta);
  for (int i = 0; i < binN; i++)
  {
    const vector<CostfIdx> &similari = similar[i];
    float saliencyI = sal[i], totalW = 1;

    for (int j = 1; j < binN; j++)
    {
      float w = expf(sqr(similari[j].first)*guassCoeff);
      if (w < 1e-8f)
        break;
      saliencyI += w * sal[similari[j].second];
      totalW += w;
    }
    nSal[i] = saliencyI / totalW;
  }
  //*/
}


int ObjectSegmentation::Quantize(const Mat& img3f, Mat &idx1i, Mat &_color3f, Mat &_colorNum, double ratio)
{
  static const int clrNums[3] = {12, 12, 12};
  static const float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
  static const int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};

  CV_Assert(img3f.data != NULL);
  idx1i = Mat::zeros(img3f.size(), CV_32S);
  int rows = img3f.rows, cols = img3f.cols;

  if (img3f.isContinuous() && idx1i.isContinuous())
  {
    cols *= rows;
    rows = 1;
  }
  // Build color pallet
  map<int, int> pallet;
  for (int y = 0; y < rows; y++) {
    const float* imgData = img3f.ptr<float>(y);
    int* idx = idx1i.ptr<int>(y);
    for (int x = 0; x < cols; x++, imgData += 3)
    {
      //  LOGE("%d %d | %f %f %f ", x, y, imgData[0], imgData[1], imgData[2]); //307881
      idx[x] = (int)(imgData[0]*clrTmp[0])*w[0] + (int)(imgData[1]*clrTmp[1])*w[1] + (int)(imgData[2]*clrTmp[2]);
      pallet[idx[x]] ++;
    }
  }

  // Fine significant colors
  int maxNum = 0;
  {
    int count = 0;
    vector<pair<int, int> > num; // (num, color) pairs in num
    num.reserve(pallet.size());
    for (map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++) {
      num.push_back(pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
    }
    sort(num.begin(), num.end(), std::greater<pair<int, int> >());

    maxNum = (int)num.size();
    int maxDropNum = cvRound(rows * cols * (1-ratio));
    for (int crnt = num[maxNum-1].first; crnt < maxDropNum && maxNum > 1; maxNum--)
      crnt += num[maxNum - 2].first;
    maxNum = min(maxNum, 256); // To avoid very rarely case
    if (maxNum < 10)
      maxNum = min((int)pallet.size(), 100);
    pallet.clear();
    for (int i = 0; i < maxNum; i++) {
      pallet[num[i].second] = i;
    }

    vector<Vec3i> color3i(num.size());
    for (unsigned int i = 0; i < num.size(); i++)
    {
      color3i[i][0] = num[i].second / w[0];
      color3i[i][1] = num[i].second % w[0] / w[1];
      color3i[i][2] = num[i].second % w[1];
    }

    for (unsigned int i = maxNum; i < num.size(); i++)
    {
      int simIdx = 0, simVal = INT_MAX;
      for (int j = 0; j < maxNum; j++)
      {
        int d_ij = vecSqrDist3(color3i[i], color3i[j]);
        if (d_ij < simVal)
          simVal = d_ij, simIdx = j;
      }
      pallet[num[i].second] = pallet[num[simIdx].second];
    }
  }

  _color3f = Mat::zeros(1, maxNum, CV_32FC3);
  _colorNum = Mat::zeros(_color3f.size(), CV_32S);
  Vec3f* color = (Vec3f*)(_color3f.data);
  int* colorNum = (int*)(_colorNum.data);
  for (int y = 0; y < rows; y++)
  {
    const Vec3f* imgData = img3f.ptr<Vec3f>(y);
    int* idx = idx1i.ptr<int>(y);
    for (int x = 0; x < cols; x++)
    {
      idx[x] = pallet[idx[x]];
      color[idx[x]] += imgData[x];
      colorNum[idx[x]] ++;
    }
  }
  for (int i = 0; i < _color3f.cols; i++) {
    vecDiv(color[i], colorNum[i]);
  }

  return _color3f.cols;
}

//==================================================

Mat ObjectSegmentation::GetRC(const Mat &img3f)
{
  return GetRC(img3f, 0.4, 50, 50, 0.5);
}

Mat ObjectSegmentation::GetRC(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma)
{
  Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
  if (Quantize(img3f, colorIdx1i, color3fv, tmp) <= 2) // Color quantization
    return Mat::zeros(img3f.size(), CV_32F);
  cvtColor(img3f, _img3f, CV_BGR2Lab);
  cvtColor(color3fv, color3fv, CV_BGR2Lab);
  int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);
  vector<Region> regs(regNum);
  BuildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
  RegionContrast(regs, color3fv, regSal1v, sigmaDist);

  Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
  cv::normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
  float* regSal = (float*)regSal1v.data;
  for (int r = 0; r < img3f.rows; r++){
    const int* regIdx = regIdx1i.ptr<int>(r);
    float* sal = sal1f.ptr<float>(r);
    for (int c = 0; c < img3f.cols; c++)
      sal[c] = regSal[regIdx[c]];
  }
  GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
  return sal1f;
}

void ObjectSegmentation::BuildRegions(const Mat& regIdx1i, vector<Region> &regs, const Mat &colorIdx1i, int colorNum)
{
  int rows = regIdx1i.rows, cols = regIdx1i.cols, regNum = (int)regs.size();
  Mat_<int> regColorFre1i = Mat_<int>::zeros(regNum, colorNum); // region color frequency
  for (int y = 0; y < rows; y++){
    const int *regIdx = regIdx1i.ptr<int>(y);
    const int *colorIdx = colorIdx1i.ptr<int>(y);
    for (int x = 0; x < cols; x++, regIdx++, colorIdx++){
      Region &reg = regs[*regIdx];
      reg.pixNum ++;
      reg.centroid.x += x;
      reg.centroid.y += y;
      regColorFre1i(*regIdx, *colorIdx)++;
    }
  }

  for (int i = 0; i < regNum; i++){
    Region &reg = regs[i];
    reg.centroid.x /= reg.pixNum * cols;
    reg.centroid.y /= reg.pixNum * rows;
    int *regColorFre = regColorFre1i.ptr<int>(i);
    for (int j = 0; j < colorNum; j++){
      float fre = (float)regColorFre[j]/(float)reg.pixNum;
      if (regColorFre[j])
        reg.freIdx.push_back(make_pair(fre, j));
    }
  }
}

void ObjectSegmentation::RegionContrast(const vector<Region> &regs, const Mat &color3fv, Mat& regSal1d, double sigmaDist)
{
  Mat_<float> cDistCache1f = Mat::zeros(color3fv.cols, color3fv.cols, CV_32F);{
    Vec3f* pColor = (Vec3f*)color3fv.data;
    for(int i = 0; i < cDistCache1f.rows; i++)
      for(int j= i+1; j < cDistCache1f.cols; j++)
        cDistCache1f[i][j] = cDistCache1f[j][i] = vecDist3(pColor[i], pColor[j]);
  }

  int regNum = (int)regs.size();
  Mat_<double> rDistCache1d = Mat::zeros(regNum, regNum, CV_64F);
  regSal1d = Mat::zeros(1, regNum, CV_64F);
  double* regSal = (double*)regSal1d.data;
  for (int i = 0; i < regNum; i++){
    for (int j = 0; j < regNum; j++){
      if(i<j) {
        double dd = 0;
        const vector<CostfIdx> &c1 = regs[i].freIdx, &c2 = regs[j].freIdx;
        for (size_t m = 0; m < c1.size(); m++)
          for (size_t n = 0; n < c2.size(); n++)
            dd += cDistCache1f[c1[m].second][c2[n].second] * c1[m].first * c2[n].first;
        rDistCache1d[j][i] = rDistCache1d[i][j] = dd * exp(-pntSqrDist(regs[i].centroid, regs[j].centroid)/sigmaDist);
      }
      regSal[i] += regs[j].pixNum * rDistCache1d[i][j];
    }
  }
}