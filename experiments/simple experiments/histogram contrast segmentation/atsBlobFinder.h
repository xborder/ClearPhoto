
#include <cvaux.h>
#include <highgui.h>

using namespace cv;
using namespace std;
class atsBlobFinder
    {
    public:
        atsBlobFinder()
        {
        }
        ///Original Code by http://nghiaho.com/?p=1102
        ///Changed and added commments. Removed Errors
        ///works with VS2010 and OpenCV 2.2+
        void FindBlobs(const cv::Mat &binary, vector < vector<cv::Point>  > &blobs)
        {
            blobs.clear();

            // Fill the label_image with the blobs
            // 0  - background
            // 1  - unlabelled foreground
            // 2+ - labelled foreground

            ///input is a binary image therefore values are either 0 or 1
            ///out objective is to find a set of 1's that are together and assign 2 to it
            ///then look for other 1's, and assign 3 to it....so on a soforth

            cv::Mat label_image;
            binary.convertTo(label_image, CV_32FC1); // weird it doesn't support CV_32S! Because the CV::SCALAR is a double value in the function floodfill

            int label_count = 2; // starts at 2 because 0,1 are used already

            //erode to remove noise-------------------------------
            Mat element = getStructuringElement( MORPH_RECT,
            Size( 2*3 + 1, 2*3+1 ),
            Point( 0, 0 ) );
            /// Apply the erosion operation
            erode( label_image, label_image, element );
            //---------------------------------------------------

            //just check the Matrix of label_image to make sure we have 0 and 1 only
            //cout << label_image << endl;

            for(int y=0; y < binary.rows; y++) {
                for(int x=0; x < binary.cols; x++) {
                    float checker = label_image.at<float>(y,x); //need to look for float and not int as the scalar value is of type double
                    cv::Rect rect;
                    //cout << "check:" << checker << endl;
                    if(checker == 1) {
                        //fill region from a point
                        cv::floodFill(label_image, cv::Point(x,y), cv::Scalar(label_count), &rect, cv::Scalar(0), cv::Scalar(0), 4);
                        label_count++;
                        //cout << label_image << endl <<"by checking: " << label_image.at<float>(y,x) <<endl;
                        //cout << label_image;

                        //a vector of all points in a blob
                        std::vector<cv::Point> blob;

                        for(int i=rect.y; i < (rect.y+rect.height); i++) {
                            for(int j=rect.x; j < (rect.x+rect.width); j++) {
                                float chk = label_image.at<float>(i,j);
                                //cout << chk << endl;
                                if(chk == label_count-1) {
                                    blob.push_back(cv::Point(j,i));
                                }                        
                            }
                        }
                        //place the points of a single blob in a grouping
                        //a vector of vector points
                        blobs.push_back(blob);
                    }
                }
            }
            cout << label_count <<endl;
            imshow("label image",label_image);
        }
    private:
    }; 