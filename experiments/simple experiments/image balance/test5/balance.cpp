#include "balance.h"

vector<vector<Point> > divideRegions(Mat& image, Mat& clustered);
vector<Point> generateClusters(Mat image, Mat clustered, int subRegionWidth, int subRegionHeight );
double resemblance(vector<Point> r1, vector<Point> r2, int subRegionHeight, int subRegionWidth);

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
  Point center;
  if(count > 0) 
    center = Point(x/count, y/count);
  else
    center = Point(0,0);
  return center;
}

double resemblance(vector<Point> r1, vector<Point> r2, int subRegionHeight, int subRegionWidth) {

  Mat shape1 = Mat::zeros(subRegionHeight, subRegionWidth, CV_8UC1);
  Mat shape2 = Mat::zeros(subRegionHeight, subRegionWidth, CV_8UC1);

  fillConvexPoly(shape1, r1, Scalar(255));
  fillConvexPoly(shape2, r2, Scalar(255));

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  findContours(shape1, contours, hierarchy,
               CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);

  drawContours(shape1, contours, -1, Scalar(255, 255, 255));


  vector<vector<Point> > contours2;
  vector<Vec4i> hierarchy2;

  findContours(shape2, contours2, hierarchy2,
               CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);

  drawContours(shape2, contours2, -1, Scalar(255, 255, 255));

  double match = matchShapes(contours[0], contours2[0], CV_CONTOURS_MATCH_I3,0);
  return match;
}
vector<vector<Point> > divideRegions(Mat& image, Mat& clustered) {
  int subRegionWidth = image.cols/3, subRegionHeight = image.rows/3;
  vector<vector<Point> > distances;
  Rect topLeftRegion(Point(0,0), Point(subRegionWidth, subRegionHeight));
  Rect topMiddleRegion(Point(subRegionWidth, 0), Point(2*subRegionWidth, subRegionHeight));
  Rect topRightRegion(Point(2*subRegionWidth, 0), Point(3*subRegionWidth, subRegionHeight));

  Rect midLeftRegion(Point(0, subRegionHeight), Point(subRegionWidth, 2*subRegionHeight));
  Rect midMiddleRegion(Point(subRegionWidth, subRegionHeight), Point(2*subRegionWidth, 2*subRegionHeight));
  Rect midRightRegion(Point(2*subRegionWidth, subRegionHeight), Point(3*subRegionWidth, 2*subRegionHeight));

  Rect bottomLeftRegion(Point(0, 2*subRegionHeight), Point(subRegionWidth, 3*subRegionHeight));
  Rect bottomMiddleRegion(Point(subRegionWidth, 2*subRegionHeight), Point(2*subRegionWidth, 3*subRegionHeight));
  Rect bottomRightRegion(Point(2*subRegionWidth, 2*subRegionHeight), Point(3*subRegionWidth, 3*subRegionHeight));


 // Mat clustered(image.rows, image.cols, CV_32FC1);

  vector<Point> r1 = generateClusters(image(topLeftRegion), clustered(topLeftRegion), subRegionWidth, subRegionHeight);
  vector<Point> r2 = generateClusters(image(topMiddleRegion), clustered(topMiddleRegion), subRegionWidth, subRegionHeight);
  vector<Point> r3 = generateClusters(image(topRightRegion), clustered(topRightRegion), subRegionWidth, subRegionHeight);

  vector<Point> r4 = generateClusters(image(midLeftRegion), clustered(midLeftRegion), subRegionWidth, subRegionHeight);
  vector<Point> r5 = generateClusters(image(midMiddleRegion), clustered(midMiddleRegion), subRegionWidth, subRegionHeight);
  vector<Point> r6 = generateClusters(image(midRightRegion), clustered(midRightRegion), subRegionWidth, subRegionHeight);

  vector<Point> r7 = generateClusters(image(bottomLeftRegion), clustered(bottomLeftRegion), subRegionWidth, subRegionHeight);
  vector<Point> r8 = generateClusters(image(bottomMiddleRegion), clustered(bottomMiddleRegion), subRegionWidth, subRegionHeight);
  vector<Point> r9 = generateClusters(image(bottomRightRegion), clustered(bottomRightRegion), subRegionWidth, subRegionHeight);

  distances.push_back(r1);
  distances.push_back(r2);
  distances.push_back(r3);
  distances.push_back(r4);
  distances.push_back(r5);
  distances.push_back(r6);
  distances.push_back(r7);
  distances.push_back(r8);
  distances.push_back(r9);

  rectangle(image, topLeftRegion, Scalar::all(255), 2);
  rectangle(image, topMiddleRegion, Scalar::all(255), 2);
  rectangle(image, topRightRegion, Scalar::all(255), 2);
  rectangle(image, midLeftRegion, Scalar::all(255), 2);
  rectangle(image, midMiddleRegion, Scalar::all(255), 2);
  rectangle(image, midRightRegion, Scalar::all(255), 2);
  rectangle(image, bottomLeftRegion, Scalar::all(255), 2);
  rectangle(image, bottomMiddleRegion, Scalar::all(255), 2);
  rectangle(image, bottomRightRegion, Scalar::all(255), 2);

  //clustered.convertTo(clustered, CV_8UC1);
  return distances;
}

vector<Point> generateClusters(Mat image, Mat clustered, int subRegionWidth, int subRegionHeight ) {
  RNG rng(12345);
  
  //Collecting samples for kmeans;
  Mat samples(subRegionHeight * subRegionWidth, 3,  CV_32F);
  
  vector<Mat> bgr;
  cv::split(image, bgr);

  for(int i=0; i < subRegionHeight*subRegionWidth; i++) {
    samples.at<float>(i,0) = bgr[0].data[i] / 255.0;
    samples.at<float>(i,1) = bgr[1].data[i] / 255.0;
    samples.at<float>(i,2) = bgr[2].data[i] / 255.0;
  }
  int clusterCount = 3;
  Mat labels;
  int attempts = 5;
  Mat cent;
  kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, cent );
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

void ImageBalance::getBalance(const Mat& image) {
  Mat canny, blur;

  Rect leftRegion(Point(0,0), Point(image.cols/2, image.rows));
  Rect rightRegion(Point(image.cols/2,0), Point(image.cols, image.rows));
  /*image.convertTo(image, CV_BGR2HSV);

  vector<Mat> tmp;
  split(image, tmp);

  Mat hue = tmp[0];*/
  Mat right;
  Mat left = image(leftRegion).clone();
  Mat flipped = image(rightRegion).clone();
  flip(flipped, right, 1);

  Mat clustered(image.rows, image.cols, CV_32FC1);
  Mat cl = clustered(leftRegion).clone();
  Mat cr = clustered(rightRegion).clone();
  vector<vector<Point> > c1= divideRegions(left, cl);
  vector<vector<Point> > c2 = divideRegions(right, cr);

  cl.convertTo(cl, CV_8UC1);
  cr.convertTo(cr, CV_8UC1);

  int subRegionWidth = left.cols/3, subRegionHeight = left.rows/3;
  double sum_diff = 0.0, sum_resemblance = 0.0;
  for(int i = 0; i < c1.size(); i++) {
    double matching = resemblance(c1[i], c2[i], subRegionHeight, subRegionWidth);
    sum_resemblance += matching;
    int sum = 0;
    cout << matching << endl;
    for(int j = 0; j < c1[i].size(); j++) {
      sum += sqrt(pow(c1[i][j].x - c2[i][j].x,2) + pow(c1[i][j].y - c2[i][j].y,2));
    }
    sum_diff += matching*sum;
  }

  cout << "Resemblance " << sum_resemblance << " | Sum diff: " << ((sum_diff)) << " | " << (sum_diff/(image.rows*image.cols))*100 << endl;
  //imshow("c left", c1);
  //imshow("c right", c2);
  imshow("left", left);
  imshow("right", right);
  imshow("ccc", cl);
  imshow("sccc", cr);
  waitKey(0);
}
