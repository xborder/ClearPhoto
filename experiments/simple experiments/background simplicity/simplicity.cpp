#include "simplicity.h"


int getWidthOfMass(vector<float> vec, float total, int maxIdx);

int main( int argc, char** argv ) {
  // check for supplied argument
  if( argc < 2 ) {
    cout << "Usage: loadimg <filename>\n" << endl;
    return 1;
  }
  // load the image, load the image in grayscale
  Mat img = imread( argv[1], CV_LOAD_IMAGE_COLOR );
  // always check
  if( img.data == NULL ) {
    cout << "Cannot load file " << argv[1] << endl;
    return 1;
  }

  //BackgroundSimplicity::method1(img);
  //BackgroundSimplicity::method2(img);
  BackgroundSimplicity::method3(img);

  return 0;
}

//Photo and Video Quality Evaluation: Focusing on the Subject

void BackgroundSimplicity::method1(Mat& image){
  Mat tmp;
  image.convertTo(tmp, CV_32FC3, 1.0/255);
  static const int nbins = 16;
  static const int clrNums[3] = {nbins, nbins, nbins};
  static const float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
  static const int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};

  int rows = image.rows, cols = image.cols;

  // Build color pallet
  int max = 0;
  map<int, int> pallet;
  for (int y = 0; y < rows; y++) {
    const float* imgData = tmp.ptr<float>(y);
    for (int x = 0; x < cols; x++, imgData += 3) {
      int idx = (int)(imgData[0]*clrTmp[0])*w[0] + (int)(imgData[1]*clrTmp[1])*w[1] + (int)(imgData[2]*clrTmp[2]);
      pallet[idx] ++;
      max = (pallet[idx] > max) ? pallet[idx] : max;
    }
  }
  vector<pair<int, int> > num; // (num, color) pairs in num
  num.reserve(pallet.size());
  for (map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++) {
    num.push_back(pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
  }
  sort(num.begin(), num.end(), std::greater<pair<int, int> >());

  static const double psy = 0.01;

  int nelems = 0;
  for (int i = 0; num[i].first > max * psy; i++)
    nelems++;

  double simplicity_factor = (nelems/4096.0) * 100;

  cout << simplicity_factor << endl;
}	

//Automatic Aesthetic Photo - Rating System

void BackgroundSimplicity::method2(Mat& image) {
  int ddepth = CV_16S;
  int scale = 1;
  int delta = 0;

  Mat tmp, gray;
  GaussianBlur( image, tmp, Size(3,3), 0, 0, BORDER_DEFAULT );

  cvtColor( tmp, gray, CV_BGR2GRAY );

  Mat grad_x, grad_y, grad;
  Mat abs_grad_x, abs_grad_y;

  Scharr( gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );

  Scharr( gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_y, abs_grad_y );


  /// Total Gradient (approximate)
  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

  int edge_count = 0;
  for (int y = 0; y < grad.rows; y++) {
    uchar* imgData = grad.ptr<uchar>(y);
    for (int x = 0; x < grad.cols; x++) {
      if(imgData[x] > 100)
        edge_count++;
    }
  }

  printf("%d %d %f\n", edge_count, image.rows*image.cols, (double)edge_count/(image.rows*image.cols));
  imshow( "result", grad );



  waitKey(0);
}

void BackgroundSimplicity::method3(Mat& image) {
  static int kernel_size = 3;
  static int scale = 1;
  static int delta = 0;
  static int ddepth = CV_16S;
  static int alpha = 0.2;

  static Mat kernel = (Mat_<float>(3,3) <<
                          alpha,        (1 - alpha),    alpha,
                          (1 - alpha),  -4,           (1-alpha),
                          alpha,        (1 - alpha),    alpha);
  static Mat laplace_kernel =  (Mat_<float>(3,3) << 
                                   0, -1,  0,
                                  -1,  4, -1,
                                   0, -1,  0);
  //static Mat laplace_filter2 = (Mat_<float>(3,3) << 
  //                                -1, -1, -1,
  //                                -1,  8, -1,
  //                                -1, -1, -1);

  vector<Mat> bgr_planes;
  split( image, bgr_planes );

  //Generate laplacian images for color images
  vector<Mat> laplacian;
  Mat tmp, abs_tmp;
  for(int i = 0; i < bgr_planes.size(); i++) {
    filter2D(bgr_planes[i], tmp, ddepth, kernel);
    convertScaleAbs( tmp, abs_tmp );
    laplacian.push_back( abs_tmp.clone() );
  }

  //Mean across laplacian images
  Mat laplacian_avg = Mat::zeros(image.size(), CV_8U);
  for(int i = 0; i < laplacian.size(); i++) {
    laplacian_avg += laplacian[i];    
  }
  laplacian_avg /= laplacian.size();

  //Edge Simplification
  uchar* laplacian_data = (uchar*)laplacian_avg.data;
  for(int x = 0; x < laplacian_avg.cols; x++) {
      for(int y = 0; y < laplacian_avg.rows; y++) {
        if(laplacian_data[y * laplacian_avg.cols + x] < 50)
            laplacian_data[y * laplacian_avg.cols + x] = 0;
      }
  }

  //Resize
  Mat resized;
  resize(laplacian_avg, resized, Size(100,100));
  resized.convertTo(resized, CV_32F, 1.0/255); //Normalization to [0,1]

  //Creating Px and Py vector, total sum calculation and finding max value index calculation
  Mat edge_distr(resized.size(), CV_32F);
  vector<float> px(resized.cols), py(resized.rows);
  float* data = (float*)resized.data;
  float sum = 0.0, total_x = 0.0, total_y = 0.0;
  int max_x = 0, max_y = 0, max_sum = 0;

  for(int x = 0; x < resized.cols; x++) {
      for(int y = 0; y < resized.rows; y++) {
            sum += data[y*resized.cols + x];
            total_x += data[y*resized.cols + x];
      }
      px[x] = sum;
      if(sum > max_sum) {
        max_sum = sum;
        max_x = x;
      }
      sum = 0.0;
  }

  max_sum = 0;
  for(int y = 0; y < resized.rows; y++) {
      for(int x = 0; x < resized.cols; x++) {
        sum += data[y*resized.cols + x];
        total_y += data[y*resized.cols + x];
      }
      py[y] = sum;
      if(sum > max_sum) {
        max_sum = sum;
        max_y = y;
      }
      sum = 0.0;
  }

  //get width of 98% of mass
  int wx = getWidthOfMass(px, total_x, max_x);
  int wy = getWidthOfMass(py, total_y, max_y);

  cout << "Final result: " << (1-(wx*wy)/(float)10000) << endl;

  /// Show what you got
  imshow( "result", laplacian_avg);

  waitKey(0);

}

int BackgroundSimplicity::getWidthOfMass(vector<float> vec, float total, int maxIdx) {
  int offset = 1;
  int initIdx = maxIdx, endIdx = maxIdx;
  float acum = vec[maxIdx], tmp = vec[maxIdx];
  while(acum < total * 0.98 && (maxIdx + offset < vec.size() || maxIdx - offset >= 0)) {
    if(maxIdx - offset >= 0) {
      tmp += vec[maxIdx - offset];
      if(tmp > total * 0.98) break;
      else {
        acum = tmp;
        initIdx = maxIdx - offset;
      }
    } 
    if(maxIdx + offset < vec.size()) {
      tmp += vec[maxIdx + offset];
      if(tmp > total * 0.98) break;
      else {
        acum = tmp;
        endIdx = maxIdx + offset;
      }
    }
    offset++;
  }
  //cout << total << " " << (total * 0.98) << endl;
  //cout << acum << " " << initIdx << " " << endIdx <<  " " << endIdx - initIdx << endl;
  return endIdx - initIdx;
}