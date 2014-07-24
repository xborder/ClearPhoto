#include "balance.h"

#define DEG2RAD 0.017453293f

priority_queue<HoughPoint, vector<HoughPoint>, HoughPointCompare> getHoughPoints(Mat& img);

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

Point getCenterMass(const Mat& img) {
  int x = 0, y = 0, count = 0;
  for (int r = 0; r < img.rows; r++) {
    const uchar* binary_row = img.ptr<uchar>(r);
    for (int c = 0; c < img.cols; c++) {
      if(binary_row[c] != 0) {
        x += c;
        y += r;
        count++;
      }
    }
  }
  return Point(x/count, y/count);
}

void ImageBalance::getBalance(const Mat& image) {
  Mat canny, blur;

  Rect leftRegion(Point(0,0), Point(image.cols/2, image.rows));
  Rect rightRegion(Point(image.cols/2,0), Point(image.cols, image.rows));

  Mat right;
  Mat left = image(leftRegion);
  Mat flipped = image(rightRegion);
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
  filter2D(blur, left, ddepth, kernel);
  cv::blur( right, blur, cv::Size(5,5) );
  filter2D(blur, right, ddepth, kernel);
  
  /*
  Canny( blur, left, 100, 150, 3 );
  Canny( blur, right, 100, 150, 3 );
*/
  Point leftCenter = getCenterMass(left);
  Point rightCenter = getCenterMass(right);

  circle(left, leftCenter, 2, Scalar::all(255),20);
  circle(right, rightCenter, 2, Scalar::all(255),20);

  imshow("left", left);
  imshow("right", right);

  float distance = sqrt(pow(leftCenter.x - rightCenter.x, 2) + pow(leftCenter.y - rightCenter.y, 2));

  printf("%f\n", distance);
  waitKey(0);
}