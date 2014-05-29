#include <stdio.h>
#include <cv.h>
#include <highgui.h>
 
int main( int argc, char **argv ){
    IplImage *img, *hsv, *hue, *sat, *val;
    int key = 0, depth;
    CvSize size;
 
    /* Load command line passed image, check it. */
    if (argc>1) {
        img = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
        if(!img){
            printf("Could not open image.");
            exit -1;
        }
        if(img->nChannels!=3){
            printf("We need color image!");
            exit -1;
        }
    } else {
        printf("Usage: %s VIDEO_FILE\n", argv[0]);
        return 1;
    }
 
    /* Create a hsv image with 3 channels and hue, sat e val with 1 channel.
       All with the same size */
    size = cvGetSize(img);
    depth = img->depth;
    hue = cvCreateImage(size, depth, 1);
    sat = cvCreateImage(size, depth, 1);
    val = cvCreateImage(size, depth, 1);
    hsv = cvCreateImage(size, depth, 3);
    cvZero(hue);
    cvZero(sat);
    cvZero(val);
    cvZero(hsv);
 
    /* Convert from Red-Green-Blue to Hue-Saturation-Value */
    cvCvtColor( img, hsv, CV_BGR2HSV );
 
    /* Split hue, saturation and value of hsv on them */
    cvSplit(hsv, hue, sat, val, 0);

    int i = 0, sum = 0;
    for(i = 0; i < size.height*size.width; i++ ) {
	sum += (unsigned char)sat->imageData[i];
    }
    printf("width: %d, height: %d, sum: %d, avg: %d\n", size.width, size.height,sum, sum/(size.width*size.height));
    /* Create windows, display them, wait for a key */
//    cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
//    cvNamedWindow("hue", CV_WINDOW_AUTOSIZE);
//    cvNamedWindow("saturation", CV_WINDOW_AUTOSIZE);
//    cvNamedWindow("value", CV_WINDOW_AUTOSIZE);
 
//    cvShowImage("original", img);
//    cvShowImage("hue", hue);
//    cvShowImage("saturation", sat);
//    cvShowImage("value", val);
 
//    cvWaitKey(0);
 
    /* Free memory and get out */
//    cvDestroyWindow("original");
//   cvDestroyWindow("hue");
//    cvDestroyWindow("saturation");
//    cvDestroyWindow("value");
 
    cvReleaseImage(&img);
    cvReleaseImage(&hsv);
    cvReleaseImage(&hue);
    cvReleaseImage(&sat);
    cvReleaseImage(&val);
 
    return 0;
}
