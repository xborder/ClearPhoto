#include "ColorTemplate.h"

vector<int> ColorTemplate::getTemplate(Mat& data) {
	static int nBins = 12;
	static int colorRange = 359;
	Mat hsv;
	data.convertTo(hsv, CV_32FC3);
	cvtColor(hsv, hsv, CV_BGR2HSV_FULL);

	vector<Mat> channels(3);
	split(hsv, channels);

	Mat histogram  = getHistogram(channels[0], nBins, colorRange);

	vector<int> bins = orderHistogram(histogram);

	return bins;
}

Mat ColorTemplate::getHistogram(Mat image, int hSize, int rangeI) {
	int histSize = hSize;

	float range[] = { 0.0f, (float)rangeI } ;
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat h_hist;

	/// Compute the histograms:
	calcHist(&image, 1, 0, Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate );

	return h_hist;
}

vector<int> ColorTemplate::orderHistogram(Mat& histogram) {
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
	}

	//is it monochramatic or complementary?
	int first = sumBin[0].second;
	float firstOccupation = sumBin[0].first/(float)sum*100;
	int second = sumBin[1].second;
	float secondOccupation = sumBin[1].first/(float)sum*100;
	int diff = abs(first*30 - second*30);

	vector<int>  binsToOccupy;

	if( diff > 150 && diff <= 210) {
		//complementary
		vector<int> firstBins = test90DegreesCluster(pallet, sumBin, sum, 0);
		vector<int> secondBins = test90DegreesCluster(pallet, sumBin, sum, 1);

		binsToOccupy.insert( binsToOccupy.end(), firstBins.begin(), firstBins.end() );
		binsToOccupy.insert( binsToOccupy.end(), secondBins.begin(), secondBins.end() );

	} else if ( diff > 0 && diff <= 90) {
		//monochromatic
		vector<int> bins = test90DegreesCluster(pallet, sumBin, sum, 0);
		binsToOccupy.insert( binsToOccupy.end(), bins.begin(), bins.end() );
	}
	return binsToOccupy;
}

vector<int> ColorTemplate::test90DegreesCluster(map<int,int> pallet, vector<pair<int,int> > sumBin, int sum, int startIndex) {
	vector<int> binsToOccupy;
	int first = sumBin[startIndex].second;
	float largest_occupation = sumBin[startIndex].first /(float)sum * 100;
	float avg_1 = 0.0;
	int count = 0;

	vector<int> avg_1_bins;
	for(int i = first; count < 3; i = mod(i + 1, 12)) {
		float bin_occupation = pallet[i]/(float)sum*100;
		if(bin_occupation < largest_occupation * 0.40)
			break;
		avg_1 += bin_occupation;
		avg_1_bins.push_back(i);
		count++;
	}
	if(count != 0) {
		avg_1 = avg_1/count;
		binsToOccupy = avg_1_bins;
	}

	vector<int> avg_2_bins;
	float avg_2 = 0.0;
	count = 0;
	for(int i = first; count < 3; i = mod(i - 1, 12)) {
		float bin_occupation = pallet[i]/(float)sum*100;
		if(bin_occupation < largest_occupation * 0.4)
			break;
		avg_2 += bin_occupation;
		count++;
		avg_2_bins.push_back(i);
	}
	if(count != 0) {
		avg_2 = avg_2/count;
		if(avg_2_bins.size() > binsToOccupy.size()) {
			binsToOccupy = avg_2_bins;
		}
	}

	vector<int> avg_3_bins;
	float avg_3 = 0.0;
	count = 0;
	for(int i = mod(first - 1, 12); count < 3; i = mod(i + 1, 12)) {
		float bin_occupation = pallet[i]/(float)sum*100;
		if(bin_occupation < largest_occupation * 0.4)
			break;
		avg_3 += bin_occupation;
		count++;
		avg_3_bins.push_back(i);
	}
	if(count != 0) {
		avg_3 = avg_3/count;
		if(avg_3_bins.size() > binsToOccupy.size()) {
			binsToOccupy = avg_3_bins;
		}
	}
	return binsToOccupy;
}

