// arduinoComm.cpp : main project file.

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int menu();

/** Function Headers */
void detectAndDisplay( IplImage *img);

/** Global variables */
cv::String face_cascade_name = "haarcascade_frontalface_alt.xml";
//String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CvHaarClassifierCascade *facecascade;
CvHaarClassifierCascade *eyecascade;
CvMemStorage            *storage;
//CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
int resX=320;
int resY=240;


int main( int argc, char** argv )
{
	//TODO - menu will return a value corresponding to a mode to enter.
	int menucode;
	menucode=menu();

	//-- 1. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -2; };
	storage = cvCreateMemStorage( 0 );
	char      *filenameFace = "haarcascade_frontalface_alt.xml";
	char      *filenameEyes = "haarcascade_eye_tree_eyeglasses.xml";
	facecascade = ( CvHaarClassifierCascade* )cvLoad( filenameFace, 0, 0, 0 );
	eyecascade = ( CvHaarClassifierCascade* )cvLoad( filenameEyes, 0, 0, 0 );

	//-- 2. Read the video stream
	CvCapture* capture;
	capture = cvCaptureFromCAM( -1 );
	int centerx, centery;
    if( capture != NULL )
	{
		//set the size of the video frame displayed to users to 80px by 60px. If you want to increase it, do it in multiples, like 160x120 or 320x240
		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, resX );
		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, resY );
		//continuously call detectanddisplay function to detect face, display video, and send info to arduino
		while( true )
		{
			//Mat frame;
			IplImage  *frame;
			frame = cvQueryFrame( capture );
			cvFlip( frame, frame, 1 );
			frame->origin = 0;
 
			
			//-- 3. Apply the classifier to the frame
			if( frame )
			{ 
				// detect faces and display video and send coordinates to arduino
				detectAndDisplay( frame);
				
			}
			else
			{ printf(" --(!) No captured frame -- Break!"); break; }

			int c = waitKey(10);
			if( (char)c == 'c' ) { break; }
		}
	}
	
	return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( IplImage *img)
{
    int i,centerx,centery;
 
    // detect faces 
    CvSeq *faces = cvHaarDetectObjects(
            img,
            facecascade,
            storage,
            1.3,
            1,
            CV_HAAR_DO_CANNY_PRUNING,
            cvSize( 40, 40 ) );
	CvRect *r;
    // for each face found, draw a red box 

    for( i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ ) {
        r = ( CvRect* )cvGetSeqElem( faces, i );
		Point center( r->x + r->width*0.5, r->y + r->height*0.5 );
		cvEllipse( img, center, Size( r->width*0.5, r->height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
		centerx = r->x + r->width*0.5;
		centery = r->y + r->height*0.5;
		//-- Future code for detecting eyes
		//IplImage *faceROI = frame_gray( faces[i] );
		/*CvSeq *eyes = cvHaarDetectObjects(
            faceROI,
            facecascade,
            storage,
            1.3,
            1,
            CV_HAAR_DO_CANNY_PRUNING,
            cvSize( 40, 40 ) );*/

		//displays rectangle for face tracking instead of ellipse
        /*cvRectangle( img,
                     cvPoint( r->x, r->y ),
                     cvPoint( r->x + r->width, r->y + r->height ),
                     CV_RGB( 255, 0, 0 ), 1, 8, 0 );*/
    }
 
    // display video 
    cvShowImage( "video", img );
}



//TODO - return a value corresponding to the mode the user selected
int menu()
{
	//determine resolution to display webcam video at
	std::cout << "Please select a video resolution:\n1. 160x120\n2. 320x240\n3. 480x360\n4. 640x480" << std::endl;
	int res;
	std::cin >> res;
	switch(res) {
		case 1:	//160x120
			resX=160;
			resY=120;
			break;
		case 2:	//320x240
			resX=320;
			resY=240;
			break;
		case 3:	//480x360
			resX=480;
			resY=360;
			break;
		case 4:	//640x480
			resX=640;
			resY=480;
			break;
	}
	std::cout << "Select a face tracking control mode" << std::endl;
	std::cout << "1. Continuous(camera moves with head always)" << std::endl;
	std::cout << "2. Directional(camera stays in last position if head returned to center)" << std::endl;
	std::cout << "3. Quit" << std::endl;
	int mode;
	std::cin >> mode;

	//std::cout << mode << endl;
	return mode;
}
