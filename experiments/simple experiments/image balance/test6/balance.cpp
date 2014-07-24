#include "balance.h"

Mat drawHistogram(Mat image, Mat mask, int histSize, int rangeI, string name);
void HVHist(Mat& image);
IplImage* convertImageRGBtoHSV(const IplImage *imageRGB);
IplImage* convertImageHSVtoRGB(const IplImage *imageHSV);
void testColor(const Mat& image);
vector<Point> generateClusters(Mat& image, Mat& clustered );
void drawSymmetry(Mat& image, Mat& clustered, vector<Point> centers, map<int,int>& leftCount, map<int,int>& rightCount);
void drawText(Mat& image, int sum, string text, int x, int y);

int clusterCount = 2;

int main( int argc, char** argv ) {
  // check for supplied argument
  if( argc < 2 ) {
    cout << "Usage: loadimg <filename>\n" << endl;
    return 1;
  }
  // load the image, load the image in grayscale
  Mat img = imread( argv[1], 1 );
  // always check
  if( img.data == NULL ) {
    cout << "Cannot load file " << argv[1] << endl;
    return 1;
  }

  ImageBalance::getBalance(img);
  return 0;
}

Mat leftdivision , rightdivision;
void drawSymmetry(Mat& image, Mat& clustered, vector<Point> centers, map<int,int>& leftCount, map<int,int>& rightCount) {
  leftdivision = Mat::zeros(image.rows, image.cols, CV_8UC1);
  rightdivision = Mat::zeros(image.rows, image.cols, CV_8UC1);

  int colors[clusterCount];
  for(int i=0; i < clusterCount; i++) {
    colors[i] = 255/(i+1);
  }
  
//  map<int,int> leftCount, rightCount;
  leftCount.clear();
  rightCount.clear();
  for(int i = 0 ; i < clusterCount; i++) {
    leftCount.insert ( pair<int,int>(i,0) );
    rightCount.insert ( pair<int,int>(i,0) );
  }
  
  if((centers[1].x - centers[0].x) == 0) { // é recta vertical
    unsigned char *leftoutput= (unsigned char*)(leftdivision.data);
    unsigned char *rightoutput= (unsigned char*)(rightdivision.data);
    for(int x = 0; x < image.cols; x++){
      for(int y = 0; y < image.rows; y++){
        if(x < centers[0].x) { // isLeft
          for(int j=0; j < clusterCount; j++) {
            if(clustered.at<uchar>(y,x) == 255/(j+1)) {
              leftCount.at(j)++;
              break;
            }
          }
          leftoutput[image.cols * y + x] = 255;
        } else { //isRight
          for(int j=0; j < clusterCount; j++) {
            if(clustered.at<uchar>(y,x) == 255/(j+1)) {
              rightCount.at(j)++;
              break;
            }
          }
          rightoutput[image.cols * y + x] = 255;
        }
      }
    }
    drawText(image, leftCount.at(0), " cluster 1 (left)", 20, 20);
    drawText(image, leftCount.at(1), " cluster 2 (left)", 20, 40);
    drawText(image, rightCount.at(0), " cluster 1 (right)", 20, 60);
    drawText(image, rightCount.at(1), " cluster 2 (right)", 20, 80);
    line(image, Point(centers[0].x, 0), Point(centers[0].x,image.rows), Scalar(255,0,0));
    cout << "left: " << leftCount.at(0) << " " << leftCount.at(1)<< endl;
    cout << "right: " << rightCount.at(0) << " " << rightCount.at(1)<< endl;
    imshow("left division", leftdivision);
    imshow("right division", rightdivision);
    waitKey(0);
    return ;
  } 

  double m = -1/(double)((centers[1].y - centers[0].y) / (centers[1].x - centers[0].x)); 
  if ( m == 0.0) { // é recta horizontal
    unsigned char *leftoutput= (unsigned char*)(leftdivision.data);
    unsigned char *rightoutput= (unsigned char*)(rightdivision.data);
    for(int x = 0; x < image.cols; x++){
      for(int y = 0; y < image.rows; y++){
        if(y < centers[0].y) { // isLeft
          for(int j=0; j < clusterCount; j++) {
            if(clustered.at<uchar>(y,x) == 255/(j+1)) {
              leftCount.at(j)++;  
              break;
            }
          }
          leftoutput[image.cols * y + x] = 255;
        } else { //isRight
          for(int j=0; j < clusterCount; j++) {
            if(clustered.at<uchar>(y,x) == 255/(j+1)) {
              rightCount.at(j)++;break;
            }
          }
          rightoutput[image.cols * y + x] = 255;
        }
      }
    }
    line(image, Point(0,centers[0].y), Point(image.cols,centers[0].y), Scalar(255,0,0));
  } else {
    double m = -1/(double)((centers[1].y - centers[0].y) /(double) (centers[1].x - centers[0].x)); 
    int xmean = (centers[0].x + centers[1].x)/2;
    int ymean = (centers[0].y + centers[1].y)/2;
    // y - y1 = m(x - x1)
    //    y = 0;  
    int y0 = 0;
    int x0 = (int)(-ymean + m * xmean) / m;

    int y1 = image.rows;
    int x1 = (int)(y1-ymean + m * xmean) / m;

    //line(division, Point(x0,y0), Point(x1,y1), Scalar(255), 5);

    unsigned char *leftoutput= (unsigned char*)(leftdivision.data);
    unsigned char *rightoutput= (unsigned char*)(rightdivision.data);
    for(int x = 0; x < image.cols; x++){
        for(int y = 0; y < image.rows; y++){
          int intersectingY = m*x - m*x0 + y0;
          if((x0 - x1)*(y - y1) < (y0 - y1)*(x - x1)) { // isLeft
            for(int j=0; j < clusterCount; j++) {
              if(clustered.at<uchar>(y,x) == 255/(j+1)) {
                leftCount.at(j)++;
                //division.at<uchar>(y,x) = 255/(j+1);
                break;
              }
            }
            leftoutput[image.cols * y + x] = 255;
          } else { //isRight
            for(int j=0; j < clusterCount; j++) {
              if(clustered.at<uchar>(y,x) == 255/(j+1)) {
                rightCount.at(j)++;
                break;
              }
            }
            rightoutput[image.cols * y + x] = 255;
          }
        }
    }
    line(image, Point(x0,y0), Point(x1,y1), Scalar(255,0,0));
  }
    
  drawText(image, leftCount.at(0), " cluster 1 (left)", 20, 40);
  drawText(image, leftCount.at(1), " cluster 2 (left)", 20, 60);
  drawText(image, rightCount.at(0), " cluster 1 (right)", 20, 80);
  drawText(image, rightCount.at(1), " cluster 2 (right)", 20, 100);
  cout << "left: " << leftCount.at(0) << " " << leftCount.at(1)<< endl;
  cout << "right: " << rightCount.at(0) << " " << rightCount.at(1)<< endl;
  imshow("left division", leftdivision);
  imshow("right division", rightdivision);
  waitKey(0);
 }

void drawText(Mat& image, int sum, string text, int x, int y) {
  ostringstream ss;
  ss << sum;
  string s =  ss.str() + " " + text;
  putText(image, s.c_str(), Point(x,y), FONT_HERSHEY_DUPLEX, 0.5, Scalar::all(0));
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

vector<Point> centers;
void ImageBalance::getBalance(const Mat& image) {
  Rect leftRegion(Point(0,0), Point(image.cols/2, image.rows));
  Rect rightRegion(Point(image.cols/2,0), Point(image.cols, image.rows));
 
  Mat tmp;
  cvtColor(image, tmp, CV_BGR2HSV);
  //image.convertTo(tmp, CV_BGR2HSV);

  vector<Mat> hsv;
  split(tmp,hsv);

  Mat clustered(image.rows, image.cols, CV_32FC1);
  centers = generateClusters(hsv[0], clustered);
  clustered.convertTo(clustered, CV_8U);

  map<int,int> leftCount;
  map<int,int> rightCount;
  drawSymmetry(hsv[0], clustered, centers, leftCount, rightCount);

  int leftSum = leftCount.at(0) + leftCount.at(1);
  int rightSum = rightCount.at(0) + rightCount.at(1);

  testColor(image);
  if(leftSum > rightSum) {
      if(leftCount.at(0)*0.7 > rightCount.at(1) ) {
        cout << "left cluster 1 is bigger"<< endl;
      } else if (leftCount.at(1)*0.7 > rightCount.at(0)) {
        cout << "left cluster 2 is bigger"<< endl;
      }
  } else {
      if(rightCount.at(0)*0.7 > leftCount.at(1) ) {
        cout << "right cluster 1 is bigger"<< endl;
      } else if (rightCount.at(1)*0.7 > leftCount.at(0)) {
        cout << "right cluster 2 is bigger"<< endl;
      }
  }

  imshow("clustered", clustered);
  imshow("image", hsv[0]);
  waitKey(0); 
}


void testColor(const Mat& image) {
  /// Separate the image in 3 places ( B, G and R )
  Mat tmp;
  cvtColor(image, tmp, CV_BGR2HSV);
  vector<Mat> hsv_planes;
  split( tmp, hsv_planes );
  Mat h = hsv_planes[0];
  Mat v = hsv_planes[2];
  Mat hl = drawHistogram(h, leftdivision, 180, 180, string("left hue"));
  Mat hr = drawHistogram(h, rightdivision, 180, 180, string("right hue"));
  Mat vl = drawHistogram(v, leftdivision, 256, 256, string("left val"));
  Mat vr = drawHistogram(v, rightdivision, 256, 256, string("right val"));

  imshow("left hue", hl);
  imshow("right hue", hr);
  imshow("left val", vl);
  imshow("right val", vr);
  waitKey(0);
}

Mat drawHistogram(Mat image, Mat mask, int hSize, int rangeI, string name) {
  /// Establish the number of bins
  int histSize = hSize;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, rangeI } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat h_hist;

  /// Compute the histograms:
  calcHist(&image, 1, 0, mask, h_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC1);
  /// Normalize the result to [ 0, histImage.rows ]
  normalize(h_hist, h_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(h_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
  }

  /// Display
  //namedWindow(name, CV_WINDOW_AUTOSIZE );
  return histImage;
}

void HVHist(Mat& image) {
  Mat hsv;
  cvtColor(image, hsv, CV_BGR2HSV);

  // Quantize the hue to 30 levels
  // and the saturation to 32 levels
  int hbins = 179, sbins = 255;
  int histSize[] = {hbins, sbins};
  // hue varies from 0 to 179, see cvtColor
  float hranges[] = { 0, 180 };
  // saturation varies from 0 (black-gray-white) to
  // 255 (pure spectrum color)
  float sranges[] = { 0, 256 };
  const float* ranges[] = { hranges, sranges };
  MatND hist;
  // we compute the histogram from the 0-th and 1-st channels
  int channels[] = {0, 2};

  calcHist( &hsv, 1, channels, Mat(), // do not use mask
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false );
  double maxVal=0;
  minMaxLoc(hist, 0, &maxVal, 0, 0);

  int scale = 5;
  Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);

  for( int h = 0; h < hbins; h++ )
    for( int s = 0; s < sbins; s++ ) {
      float binVal = hist.at<float>(h, s);
      int intensity = cvRound(binVal*255/maxVal);
      rectangle( histImg, Point(h*scale, s*scale), Point( (h+1)*scale - 1, (s+1)*scale - 1), Scalar::all(intensity), CV_FILLED );
    }

  namedWindow( "Source", WINDOW_NORMAL);
  imshow( "Source", image);

  namedWindow( "H-S Histogram", WINDOW_NORMAL );
  imshow( "H-S Histogram", histImg );
  waitKey();
}
