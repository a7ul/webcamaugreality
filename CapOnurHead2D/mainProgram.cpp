#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480



using namespace std;
using namespace cv;

//================================================================================
// GLOBAL VARIABLES
int rcnt = 0;
// OPENCV GLOBALS
//------------------
CascadeClassifier face_cascade_detector;
//Angle Calculation Variables
int pivotxref = 0, pivotyref = 0;
float Headangle;
//Current Location Calculation Variables
int cur_x = 0, cur_y = 0;
//Image from Camera
Mat imageframe;
//Video Capture device
VideoCapture vcap(0); // open the default camera

//================================================================================
//PROTOTYPES
//OPENCV PROTOTYPES
void imageProcess(VideoCapture cap, Mat& image, int& pivotx, int& pivoty, float& angle, CascadeClassifier face_cascade, int&curx, int&cury);
//COMMON PROTOTYPES
void displayVCAM();
void MyLine(Mat img, Point start, Point endp);
//================================================================================
//MAIN FUNCTION
int main(int argc, char ** argv)
{
	//INITIALIZE OPENCV OBJECTS
	if (!vcap.isOpened())  // check if we succeeded
	{
		cout << "\nCamera Can't be opened ! Close other camera apps ";
		//usleep(5000000);
		return -1;
	}
	while (!imageframe.data)
	{
		vcap >> imageframe; // get a new frame from camera
	}
	// Load Face cascade (.xml file) for face detection
	face_cascade_detector.load("haarcascade_frontalface_alt.xml");
	cout << " Developed by Atul \n===================\n";
	cout << "\nMove back and Show your face to the webcam \nOnce Ready Press enter.";
	
	getchar();
	//Create window to display the original image
	namedWindow("Window", 1);
	while (1){
		displayVCAM();
	}


}

//============================================================================
//OPENCV

void displayVCAM()
{
	imageProcess(vcap, imageframe, pivotxref, pivotyref, Headangle, face_cascade_detector, cur_x, cur_y);
	imshow("Window", imageframe);
}



//============================================================================
//OPENCV - IMAGE PROCESSING FUNCTIONS

void MyPolygon(Mat img, std::vector<Rect> faces)
{
	int lineType = 8;
	int i = 0;
	/** Create some points */
	Point rook_points[1][3];
	rook_points[0][0] = Point(faces[i].x + faces[i].width*0.1, faces[i].y);
	rook_points[0][1] = Point(faces[i].x + faces[i].width*0.9, faces[i].y);
	rook_points[0][2] = Point(faces[i].x + faces[i].width*0.5, faces[i].y - faces[i].height);

	const Point* ppt[1] = { rook_points[0] };
	int npt[] = { 3 };

	//int x = 5, y=156, z=250;

	fillPoly(img,
		ppt,
		npt,
		1,
		Scalar(255, 255, 255),
		lineType);

	//------------------------------------------------


	Point inner[1][4];
	inner[0][0] = Point(faces[i].x + faces[i].width*0.3, faces[i].y - faces[i].height / 2);
	inner[0][3] = Point(faces[i].x + faces[i].width*0.7, faces[i].y - faces[i].height / 2);
	inner[0][1] = Point(faces[i].x + faces[i].width*0.25, faces[i].y - faces[i].height / 3);
	inner[0][2] = Point(faces[i].x + faces[i].width*0.75, faces[i].y - faces[i].height / 3);





	const Point* ppt3[1] = { inner[0] };
	int npt3[] = { 4 };

	//int x = 5, y=156, z=250;

	fillPoly(img,
		ppt3,
		npt3,
		1,
		Scalar(0, 255, 255),
		lineType);




	//---------------------------------------------------


	MyLine(img, Point(faces[i].x + faces[i].width*0.1, faces[i].y), Point(faces[i].x + faces[i].width*0.5, faces[i].y - faces[i].height));
	MyLine(img, Point(faces[i].x + faces[i].width*0.9, faces[i].y), Point(faces[i].x + faces[i].width*0.5, faces[i].y - faces[i].height));
	MyLine(img, Point(faces[i].x + faces[i].width*0.9, faces[i].y), Point(faces[i].x + faces[i].width*0.9, faces[i].y));
	MyLine(img, Point(faces[i].x + faces[i].width*0.3, faces[i].y - faces[i].height / 2), Point(faces[i].x + faces[i].width*0.7, faces[i].y - faces[i].height / 2));
	MyLine(img, Point(faces[i].x + faces[i].width*0.25, faces[i].y - faces[i].height / 3), Point(faces[i].x + faces[i].width*0.75, faces[i].y - faces[i].height / 3));

	circle(img, Point(faces[i].x + faces[i].width*0.5, faces[i].y - faces[i].height), 10, Scalar(255, 0, 255), CV_FILLED, 8, 0);
}


void imageProcess(VideoCapture cap, Mat& image, int& pivotx, int& pivoty, float& angle, CascadeClassifier face_cascade, int &curx, int &cury)
{

	cap >> image; // get a new frame from camera
	//imshow( "Original", image ); //Display original image
	resize(image, image, Size(WIDTH, HEIGHT), 0, 0, INTER_LINEAR);

	// Detect faces
	std::vector<Rect> faces;
	face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	// Draw circles on the detected faces
	for (int i = 0; i < faces.size(); i++)
	{
		//cout<<endl<<"x - "<<faces[i].x<<" y - "<<faces[i].y<<" ";
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		//ellipse( image, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

		MyPolygon(image, faces);
		int x = 5, y = 156, z = 250;

		if (rcnt == 0){
			cv::putText(image, "AUGMENTED!", cv::Point(WIDTH / 6, 50), CV_FONT_NORMAL, 1, cv::Scalar(x, y, z), 2, 8, false);
		}
		else if (rcnt == 1){
			cv::putText(image, "AUGMENTED!", cv::Point(WIDTH / 6, 50), CV_FONT_NORMAL, 1, cv::Scalar(x, z, y), 2, 8, false);
		}
		else if (rcnt == 2){
			cv::putText(image, "AUGMENTED!", cv::Point(WIDTH / 6, 50), CV_FONT_NORMAL, 1, cv::Scalar(z, x, y), 2, 8, false);
		}
/*
		if (rcnt == 0){
			cv::putText(image, "     , , , , ,", cv::Point(WIDTH / 2.2, HEIGHT - 110), CV_FONT_NORMAL, 0.45, cv::Scalar(x, z, y), 2, 8, false);
		}
		else if (rcnt == 1){
			cv::putText(image, "     , , , , ,", cv::Point(WIDTH / 2.2, HEIGHT - 110), CV_FONT_NORMAL, 0.45, cv::Scalar(x, y, z), 2, 8, false);
		}
		else if (rcnt == 2){
			cv::putText(image, "     , , , , ,", cv::Point(WIDTH / 2.2, HEIGHT - 110), CV_FONT_NORMAL, 0.45, cv::Scalar(y, x, z), 2, 8, false);
		}
		cv::putText(image, "   _ | | | | | _", cv::Point(WIDTH / 2.2, HEIGHT-85), CV_FONT_NORMAL, 0.45, cv::Scalar(230, 210, 200), 2, 8, false);
		cv::putText(image, "  { ~*~*~*~ }", cv::Point(WIDTH / 2.2, HEIGHT - 60), CV_FONT_NORMAL, 0.45, cv::Scalar(x, z, y), 2, 8, false);
		cv::putText(image, "__{ *~*~*~* } __", cv::Point(WIDTH / 2.2, HEIGHT - 35), CV_FONT_NORMAL, 0.45, cv::Scalar(230, 210, 220), 2, 8, false);
		cv::putText(image, "`----------`", cv::Point(WIDTH / 2.2, HEIGHT - 10), CV_FONT_NORMAL, 0.45, cv::Scalar(230,210, 220), 2, 8, false);
*/	
		rcnt = (rcnt + 1) % 3;
		//---------------------------------------------
		//ANGLE CALCULATION FOR HEAD
		angle = atan(((faces[i].y + faces[i].width*0.5) - pivoty) / (faces[i].x - pivotx))*180.0 / 3.14159265;
		//Correction to the angle to make y axis as reference instead of x axis
		(angle>0) ? (angle = (90 - angle)) : (angle = (-90 - angle));
		//Reinitialize for next angle detection if angle > 40
		//This means the person has moved his/her position
		//rather than only head as head cannot rotate more than 40 degrees
		//under straight posture
		if (abs(angle)>39)
		{
			pivotx = faces[i].x + faces[i].width*0.5f;
			pivoty = faces[i].y - faces[i].height*0.5f;
		}
		//FINAL ANGLE VALUE FOR HEAD
		//cout<<"angle in degree: "<<angle;
		//----------------------------------------------
		//FINAL OBJECT POSITION ON HEAD
		//curx=((WIDTH)-(faces[i].x + faces[i].width*0.5));
		//cury=faces[i].y+100;

		curx = WIDTH - center.x;
		cury = center.y;
		//cout<<"\nHEAD POS X,Y "<<curx<<" "<<cury;

		break; //Basically run the loop only once as we are dealing with only 1 face.
	}

	//imshow( "Detected Face", image ); //Display image with detection ellipse
	waitKey(1); //Wait for 1 millisec

}

void MyLine(Mat img, Point start, Point endp)
{
	int thickness = 2;
	int lineType = 8;
	line(img,
		start,
		endp,
		Scalar(0, 0, 0),
		thickness,
		lineType);
}

//END
//==========================================================================
