#include "huecount.h"

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

  int res = HueCounter::getHueCount(img);
  cout << res << endl;
  return 0;
}

int HueCounter::getHueCount(Mat& data) {

  Mat hsv;
  cvtColor(data, hsv, CV_BGR2HSV);

  vector<Mat> channels(3);
  // split img:
  split(hsv, channels);

  Mat h_norm, s_norm, v_norm;
  channels[0].convertTo(h_norm, CV_32F, 1.0/255);
  channels[1].convertTo(s_norm, CV_32F, 1.0/255);
  channels[2].convertTo(v_norm, CV_32F, 1.0/255);

  Mat mask = Mat::zeros(data.size(), CV_8U);
  for (int r = 0; r < data.rows; r++) {
    float* sat = s_norm.ptr<float>(r);
    float* val = v_norm.ptr<float>(r);
    uchar* mask_data = mask.ptr<uchar>(r);
    for (int c = 0; c < data.cols; c++) {
      if(val[c] >= 0.15 && val[c] <= 0.95 && sat[c] > 0.2){
        mask_data[c] = 255;
      }
    }
  }

  // Initialize parameters
  int bins = 20;    // bin size
  float range[] = { 0, 255 };
  const float *ranges[] = { range };
 
  // Calculate histogram
  MatND hist;
  calcHist( &channels[0], 1, 0, mask, hist, 1, &bins, ranges, true, false );
  
  double minVal; 
  double maxVal; 
  Point minLoc; 
  Point maxLoc;
  minMaxLoc(hist, &minVal, &maxVal, &minLoc, &maxLoc );
  
  int count = 0;
  for( int h = 0; h < histSize; h++ ) {
    float binVal = hist.at<float>(h);
    if(binVal > 0.05*maxVal) {
      cout<<" "<<binVal;
      count ++;
    }
  }
  cout << endl;

  return bins - count;
}


// DRAW histogram
/*
  // Show the calculated histogram in command window
    double total;
    total = data.rows * data.cols;
    for( int h = 0; h < histSize; h++ )
         {
            float binVal = hist.at<float>(h);
            cout<<" "<<binVal;
         }
 
    // Plot the histogram
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );
 
    Mat histImage( hist_h, hist_w, CV_8UC1, Scalar( 0,0,0) );
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
     
    for( int i = 1; i < histSize; i++ )
    {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
    }
 
    namedWindow( "Result", 1 );    imshow( "Result", histImage );
    */