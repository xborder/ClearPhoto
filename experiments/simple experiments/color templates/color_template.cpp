#include "color_template.h"

void test90DegreesCluster(map<int,int> pallet, vector<pair<int,int> > sumBin, int sum, int first, float& occupationPerc, vector<int>& binsToOccupy);
void test60DegreesCluster(map<int,int> pallet, vector<pair<int,int> > sumBin, int sum, int first, float& occupationPerc, vector<int>& binsToOccupy);
Mat drawHistogram(Mat image, Mat mask, int hSize, int rangeI, Mat& histogram);
void orderHistogram(Mat& histogram);
int mod(int a, int b);

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

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

  ColorTemplate::getTemplate(img);
  return 0;
}
void orderHistogram(Mat& histogram) {
  map<int, int> pallet;
  for( int i = 0; i < histogram.rows; i++ ){
    pallet[i] = cvRound(histogram.at<float>(i));
  }

  vector<pair<int, int> > sumBin; // (sum, color) pairs in sum
  sumBin.reserve(pallet.size());
  for (map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++) {
    sumBin.push_back(pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
  }
  sort(sumBin.begin(), sumBin.end(), std::greater<pair<int, int> >());
  int sum = 0;
  for(int i = 0; i < sumBin.size(); i++) {
    sum += sumBin[i].first;
    printf("(%d,%d) \n", sumBin[i].second, sumBin[i].first);
  }
  cout << sum << endl;

  //is it monochramatic or complementary?
  int first = sumBin[0].second;
  float firstOccupation = sumBin[0].first/(float)sum*100;
  int second = sumBin[1].second;
  float secondOccupation = sumBin[1].first/(float)sum*100;
  int diff = abs(first*30 - second*30);
  printf("diff: %d\n", diff);
  
  if( diff > 150 && diff <= 210) {
    //complementary
    printf("### complementary %f %f\n", firstOccupation, secondOccupation);
    float occupation = secondOccupation;
    vector<int>  binsToOccupy;

    test90DegreesCluster(pallet, sumBin, sum, 0, occupation, binsToOccupy);
    for (int i = 0; i< binsToOccupy.size(); i++)
      printf("%d ", binsToOccupy[i]);
    binsToOccupy.clear();
    test90DegreesCluster(pallet, sumBin, sum, 1, occupation, binsToOccupy);
    for (int i = 0; i< binsToOccupy.size(); i++)
      printf("%d ", binsToOccupy[i]);

  } else if ( diff > 0 && diff <= 90) {
    //monochromatic
    printf("### monochromatic\n");
    float occupation = firstOccupation;
    vector<int>  binsToOccupy;
    test90DegreesCluster(pallet, sumBin, sum, 0, occupation, binsToOccupy);
    /*if(firstOccupation - secondOccupation > 80.0) {
      occupation = firstOccupation;
      binsToOccupy.push_back(first);
    } else {
        test90DegreesCluster(pallet, sumBin, sum, first, occupation, binsToOccupy);
      if(occupation > 55.0) {
        float occupation60 = firstOccupation;
        vector<int>  binsToOccupy60;
        test60DegreesCluster(pallet, sumBin, sum,  first, occupation60, binsToOccupy60);
        /*if(occupation60 > 50.0 && firstOccupation > 50.0) {
          occupation = firstOccupation;
          binsToOccupy.clear();
          binsToOccupy.push_back(first);
        } else 
          if(occupation60 > 65.0) {
          occupation = occupation60;
          binsToOccupy = binsToOccupy60;
        }
      }
    }*/
    for (int i = 0; i< binsToOccupy.size(); i++)
      printf("%d ", binsToOccupy[i]);
  }
}


void test60DegreesCluster(map<int,int> pallet, vector<pair<int,int> > sumBin, int sum, int first, float& occupationPerc, vector<int>& binsToOccupy) {
  //int first = sumBin[0].second;
  //float occupation = sumBin[0].first /(float)sum * 100;
  float avg_1 = 0;
  int count = 0;
  cout << endl;
  printf("Largest bin: %d with (%f%%)\n", first, occupationPerc);
  cout << "#################" << endl;
  //for(int i = first; i <= first+1 && i < pallet.size(); i++) {
  vector<int> avg_1_bins;
  for(int i = first; count < 2; i = mod(i + 1, 12)) {
      float bin_occupation = pallet[i]/(float)sum*100;
      avg_1 += bin_occupation;
      count++;
      avg_1_bins.push_back(i);
      printf("bin %d with %d (%f%%)\n", i, pallet[i], bin_occupation);
  }
  avg_1 = avg_1/count;
  occupationPerc = avg_1;
  binsToOccupy = avg_1_bins;
  printf("Avg occupation: %f%%\n", avg_1);
  cout << "----------------" << endl;

  float avg_2 = 0.0;
  count = 0;
  vector<int> avg_2_bins;
  for(int i = first; count < 2; i = mod(i - 1, 12)) {
      float bin_occupation = pallet[i]/(float)sum*100;
      avg_2 += bin_occupation;
      count++;
      avg_2_bins.push_back(i);
      printf("bin %d with %d (%f%%)\n", i, pallet[i], bin_occupation);
  }
  avg_2 = avg_2 / count;
  if(avg_2 > occupationPerc) {
    occupationPerc = avg_2;
    binsToOccupy = avg_2_bins;
  }
  printf("Avg occupation: %f%%\n", avg_2);
  cout << "#################" << endl;
  cout << endl;  
}

void test90DegreesCluster(map<int,int> pallet, vector<pair<int,int> > sumBin, int sum, int startIndex, float& occupationPerc, vector<int>& binsToOccupy) {

  int first = sumBin[startIndex].second;
  float largest_occupation = sumBin[startIndex].first /(float)sum * 100;
  float avg_1 = 0.0;
  int count = 0;
  cout << endl;
  printf("Largest bin: %d with (%f%%)\n", first, largest_occupation);

  vector<int> avg_1_bins;
  for(int i = first; count < 3; i = mod(i + 1, 12)) {
    float bin_occupation = pallet[i]/(float)sum*100;
    if(bin_occupation < largest_occupation * 0.5)
      break;
    avg_1 += bin_occupation;
    avg_1_bins.push_back(i);
    count++;
    printf("bin %d with %d (%f%%)\n", i, pallet[i], bin_occupation);
  }
  if(count != 0) {
    avg_1 = avg_1/count;
    occupationPerc = avg_1;
    binsToOccupy = avg_1_bins;
  }
  printf("Avg occupation: %f%%\n", avg_1);


  cout << "----------------" << endl;
  vector<int> avg_2_bins;
  float avg_2 = 0.0;
  count = 0;
  for(int i = first; count < 3; i = mod(i - 1, 12)) {
    float bin_occupation = pallet[i]/(float)sum*100;
    if(bin_occupation < largest_occupation * 0.5)
      break;
    avg_2 += bin_occupation;
    count++;
    avg_2_bins.push_back(i);
    printf("bin %d with %d (%f%%)\n", i, pallet[i], bin_occupation);
  }
  if(count != 0) {
    avg_2 = avg_2/count;
    if(avg_2 > occupationPerc) {
      occupationPerc = avg_2;
      binsToOccupy = avg_2_bins;
    }
    printf("Avg occupation: %f%%\n", avg_2);
  }


  cout << "----------------" << endl;
  vector<int> avg_3_bins;
  float avg_3 = 0.0;
  count = 0;
  for(int i = mod(first - 1, 12); count < 3; i = mod(i + 1, 12)) {
    float bin_occupation = pallet[i]/(float)sum*100;
    if(bin_occupation < largest_occupation * 0.5)
      break;
    avg_3 += bin_occupation;
    count++;
    avg_3_bins.push_back(i);
    printf("bin %d with %d (%f%%)\n", i, pallet[i], bin_occupation);
  }
  if(count != 0) {
    avg_3 = avg_3/count;
    if(avg_3 > occupationPerc) {
      occupationPerc = avg_3;
      binsToOccupy = avg_3_bins;
    }
    printf("Avg occupation: %f%%\n", avg_3);
  }
  cout << endl;  
}

void ColorTemplate::getTemplate(Mat& data) {
  double minVal; 
  double maxVal; 
  Mat tmp;
  cvtColor(data, tmp, CV_BGR2HSV);
//  Mat tmp2;
//  data.convertTo(tmp2, CV_BGR2HSV);
  vector<Mat> ch(3), c(3);
  split(tmp, ch);
  ch[1] = 255*Mat::ones(tmp.size(), CV_8UC1);
  ch[2] = 255*Mat::ones(tmp.size(), CV_8UC1);

  merge(ch, tmp);
  imshow("hue", ch[0]);
  cvtColor(tmp, tmp, CV_HSV2BGR);
  imshow("purest colors", tmp);   
  waitKey(0);
  destroyWindow("hue");
  destroyWindow("purest colors");

  Mat hsv;
  data.convertTo(hsv, CV_32FC3);
  cvtColor(hsv, hsv, CV_BGR2HSV_FULL);

  vector<Mat> channels(3);
  split(hsv, channels);

  Mat histogram;
  Mat hue_h = drawHistogram(channels[0], Mat(), 12, 359, histogram);

  orderHistogram(histogram);

  imshow("hue_hist", hue_h);
  waitKey(0);
}

Mat drawHistogram(Mat image, Mat mask, int hSize, int rangeI, Mat& histogram) {
  /// Establish the number of bins
  cout << (image.rows*image.cols) << endl;
  int histSize = hSize;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, rangeI } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat h_hist;

  /// Compute the histograms:
  calcHist(&image, 1, 0, mask, h_hist, 1, &histSize, &histRange, uniform, accumulate );

  histogram = h_hist.clone();
  // Draw the histograms for B, G and R
  int hist_w = rangeI; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage = Mat::zeros( hist_h, hist_w, CV_8UC1);
  /// Normalize the result to [ 0, histImage.rows ]
  normalize(h_hist, h_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(h_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 1, 8, 0  );

      ostringstream sb;
      sb << h_hist.at<float>(i);
      string s =  sb.str();
      putText(histImage, s.c_str(), Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i))), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255));
  }

  return histImage;
}