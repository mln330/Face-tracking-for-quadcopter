// face_tracking.cpp : main project file.

#include "stdafx.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

using namespace System;
using namespace System::IO::Ports;

void menu();
void connect(SerialPort^ arduino);

/** Function Headers */
void detectAndDisplay( Mat frame, IplImage* frame2, SerialPort^ arduino);

/** Global variables */
cv::String face_cascade_name = "haarcascade_frontalface_alt.xml";
//String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
//CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
int initx=0;
int inity=0;
int posx=0;
int posy=0;

/* Com glob variables */
ConsoleKeyInfo cki;
	
int baudRate=9600;
bool connection=false;

int main(array<System::String ^> ^args)
{
	//TODO - menu will return a value corresponding to a mode to enter.
	menu();

	//-- 1. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -2; };
	
	SerialPort^ arduino;
	connect( arduino );

	//-- 2. Read the video stream
	CvCapture* capture;
	capture = cvCaptureFromCAM( -1 );
    if( capture != NULL )
	{
		//set the size of the video frame displayed to users to 80px by 60px. If you want to increase it, do it in multiples, like 160x120 or 320x240
		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 320 );
		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 240 );
		//continuously call detectanddisplay function to detect face, display video, and send info to arduino
		while( true )
		{
			Mat frame;
			IplImage* frame2;
			frame=frame2 = cvQueryFrame( capture );

			//-- 3. Apply the classifier to the frame
			if( !frame.empty() )
			{ detectAndDisplay( frame,frame2,arduino); }
			else
			{ printf(" --(!) No captured frame -- Break!"); break; }

			int c = waitKey(10);
			if( (char)c == 'c' ) { break; }
		}
	}
	if(connection)
		arduino->Close();
	return 0;
}
/** @function detectAndDisplay */
void detectAndDisplay( Mat frame, IplImage* frame2, SerialPort^ arduino )
{
	int difx=0;
	int dify=0;
	int centerx=0;
	int centery=0;
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.3, 1, CV_HAAR_DO_CANNY_PRUNING, Size(40, 40) );
	
	for( int i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

		centerx=(faces[i].x + 15);
		centery=(faces[i].y + 15);

		//if connection to arduino has been established, send coordinates to microcontroller over serial port
		//if(connection)
			//arduino->WriteLine(centerx+"b"+centery+"e");

		Mat faceROI = frame_gray( faces[i] );

		//code for tracking eyes
		//std::vector<Rect> eyes;

		//-- In each face, detect eyes
		//eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		
	}
	//-- Show what you got
	//mirror video
	cvFlip(frame2, NULL, 1);
	frame=frame2;
	imshow( window_name, frame );
}

//TODO - return a value corresponding to the mode the user selected
void menu()
{
	std::cout << "Select a face tracking control mode" << std::endl;
	std::cout << "1. Continuous(camera moves with head always)" << std::endl;
	std::cout << "2. Directional(camera stays in last position if head returned to center)" << std::endl;
	std::cout << "3. Quit" << std::endl;
	int mode;
	std::cin >> mode;

	std::cout << mode << endl;
}

void connect(SerialPort^ arduino)
{
	System::String^ portName;
	System::String^ response;
	//take in port name from user
	Console::WriteLine("Type in a port name and hit ENTER");
	portName=Console::ReadLine();

	// try opening port, give user the choice to keep trying to connect or just continue to face tracking
	do {
		try
		{
			arduino = gcnew SerialPort(portName, baudRate);
			arduino->Open();
			printf("Successfully connected to: %s\n",portName);
			connection=true;
			break;
		}
		catch (IO::IOException^ e  )
		{
			Console::WriteLine(e->GetType()->Name+": Port is not ready. Continue to face tracking? (yes/no)");
		}
		catch (ArgumentException^ e)
		{
			Console::WriteLine(e->GetType()->Name+": incorrect port name syntax, must start with COM/com. Continue to face tracking? (yes/no)");
		}
		response=Console::ReadLine();
		if(System::String::Compare(response,"no")==0 ||System::String::Compare(response,"No")==0 || System::String::Compare(response,"NO")==0) {
			Console::WriteLine("Type in a port name and hit ENTER");
			portName=Console::ReadLine();
		}
		else {
			connection=false;
			break;
		}
	} while(1);
}
