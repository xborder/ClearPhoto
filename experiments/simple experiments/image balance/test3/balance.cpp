#include "balance.h"

#define DEG2RAD 0.017453293f

vector<int> divideRegions(Mat& image);
void putTextCount(int x, int y, Mat& img, int val);

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

  /*circle(image, Point(center1.x + topLeftRegion.x, center1.y + topLeftRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center2.x + topMiddleRegion.x, center2.y + topMiddleRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center3.x + topRightRegion.x, center3.y + topRightRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center4.x + midLeftRegion.x, center4.y + midLeftRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center5.x + midMiddleRegion.x, center5.y + midMiddleRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center6.x + midRightRegion.x, center6.y + midRightRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center7.x + bottomLeftRegion.x, center7.y + bottomLeftRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center8.x + bottomMiddleRegion.x, center8.y + bottomLeftRegion.y), 2, Scalar::all(255),5);
  circle(image, Point(center9.x + bottomRightRegion.x, center9.y + bottomRightRegion.y), 2, Scalar::all(255),5);
*/
  return centers;
}

void putTextCount(int x, int y, Mat& img, int val) {
  ostringstream sb;
  sb << val;
  string s =  sb.str();
  putText(img, s.c_str(), Point(x,y), FONT_HERSHEY_DUPLEX, 0.5, Scalar::all(255));
}

void ImageBalance::getBalance(const Mat& image) {
  Mat canny, blur;

  Rect leftRegion(Point(0,0), Point(image.cols/2, image.rows));
  Rect rightRegion(Point(image.cols/2,0), Point(image.cols, image.rows));
  Mat tmp;
  cvtColor(image, tmp, CV_BGR2GRAY);

  Mat right;
  Mat left = tmp(leftRegion).clone();
  Mat flipped = tmp(rightRegion).clone();
  flip(flipped, right, 1);

  static int kernel_size = 3;
  static int scale = 1;
  static int delta = 0;
  static int ddepth = CV_8U;

  static Mat kernel =  (Mat_<int>(3,3) <<
                                   -1,  -1,  -1,
                                   -1,   8,  -1,
                                   -1,  -1,  -1);

  cv::blur( left, blur, cv::Size(5,5) );
  //filter2D(blur, left, ddepth, kernel);
  Canny( blur, left, 100, 300, 3 );
  cv::blur( right, blur, cv::Size(5,5) );
  //filter2D(blur, right, ddepth, kernel);
  Canny( blur, right, 100, 300, 3 );

  vector<int> leftPoints = divideRegions(left);
  vector<int> rightPoints = divideRegions(right);

  int sum = 0;
  for(int i = 0; i < leftPoints.size(); i++) {
    //Point p1 = leftPoints[i];
    //Point p2 = rightPoints[i];
    sum += abs(leftPoints[i] - rightPoints[i]);//sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y,2));
  }
  printf("%f\n", (sum/(float)(image.rows*image.cols)));
/*  for (int r = offsetY; r < offsetY + subRegionWidth; r++) {
    const uchar* binary_row = left.ptr<uchar>(r);
    for (int c = offsetX; c < offsetX + subRegonHeight; c++) {
   
    }
  }*/

  imshow("left", left);
  imshow("right", right);
  //Point center = getCenterMass(image);
  //Mat tmp = image.clone();
  //circle(tmp, center, 2, Scalar(0,0,255),5);

  imshow("image", tmp);
  moveWindow("left", 1500, 0);
  moveWindow("right", 2000, 0);
  moveWindow("image", 1500, 500);
  waitKey(0);
}