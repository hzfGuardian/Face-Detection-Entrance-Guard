
#include <opencv2/opencv.hpp>
#include <cassert>
#include <iostream>
#include <algorithm>

const char  * WINDOW_NAME  = "Face Tracker";
const int CASCADE_NAME_LEN = 2048;
char    CASCADE_NAME[CASCADE_NAME_LEN] = "haarcascade_frontalface_alt2.xml";

using namespace std;
using namespace cv;

int main (int argc, char * const argv[])
{
    const int scale = 2;

    // create all necessary instances
    cvNamedWindow (WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    CvCapture * camera = cvCreateCameraCapture (CV_CAP_ANY);
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*) cvLoad (CASCADE_NAME, 0, 0, 0);
    CvMemStorage* storage = cvCreateMemStorage(0);
    assert (storage);

    // you do own an iSight, don't you ?!?
    if (! camera)
        abort ();

    // did we load the cascade?!?
    if (! cascade)
        abort ();

    // get an initial frame and duplicate it for later work
    IplImage *  current_frame = cvQueryFrame (camera);
    IplImage *  draw_image    = cvCreateImage(cvSize (current_frame->width, current_frame->height), IPL_DEPTH_8U, 3);
    IplImage *  gray_image    = cvCreateImage(cvSize (current_frame->width, current_frame->height), IPL_DEPTH_8U, 1);
    IplImage *  small_image   = cvCreateImage(cvSize (current_frame->width / scale, current_frame->height / scale), IPL_DEPTH_8U, 1);
    assert (current_frame && gray_image && draw_image);

    // as long as there are images ...
    while (current_frame = cvQueryFrame (camera))
    {
        // convert to gray and downsize
        cvCvtColor (current_frame, gray_image, CV_BGR2GRAY);
        cvResize (gray_image, small_image, CV_INTER_LINEAR);

        // detect faces
        CvSeq* faces = cvHaarDetectObjects (small_image, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize (30, 30));

        // draw faces
        cvFlip (current_frame, draw_image, 1);

        double maxR = 0;
        CvPoint maxP;
        maxP.x = 0;
        maxP.y = 0;
        for (int i = 0; i < (faces ? faces->total : 0); i++)
        {
            CvRect* r = (CvRect*) cvGetSeqElem (faces, i);
            CvPoint center;
            int radius;
            center.x = cvRound((small_image->width - r->width*0.5 - r->x) *scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);

            //filter other smaller circles
            if (radius > maxR)
            {
            	maxR = radius;
            	maxP = center;
            }
            //cvCircle (draw_image, center, radius, CV_RGB(0,255,0), 3, 8, 0 );
        }

        if ((faces ? faces->total : 0) > 0) {
			
			cvCircle (draw_image, maxP, maxR, CV_RGB(0,255,0), 3, 8, 0 );
    
            Mat image(current_frame, 0);
            int min_x = (maxP.x - maxR > 0) ? (maxP.x - maxR) : 0;
            int min_y = (maxP.y - maxR > 0) ? (maxP.y - maxR) : 0;
            min_x = image.cols - min_x - maxR * 2;
    
            Mat src(image, Rect(min_x, min_y, maxR * 2, maxR * 2));

            resize(src, src, Size(192, 168));

            imshow("Flip", src);

            //add your recognition here... src is the face
            
        }

        //add your 
        //cvRectangle( draw_image, CvPoint(), CvPoint pt2, CV_RGB(0,255,0), 3, 8, 0);
        // just show the image
        cvShowImage (WINDOW_NAME, draw_image);

        // wait a tenth of a second for keypress and window drawing
        int key = cvWaitKey (100);
        if (key == 'q' || key == 'Q')
            break;
    }

    // be nice and return no error
    return 0;
}



