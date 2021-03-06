// CTRL-M-O for at skjule alle functioner

#include "stdafx.h"
#include <cstdio>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;
using namespace std;

Mat img_grey, final, thres, thres_white, im_with_keypoints, canny, impurity_thres_b, impurity_thres_w, impurity_final, 
injuries_green, injuries_red, injuries_thres_g, injuries_thres_r, injuries_g_final, injuries_r_final;
//Mat img = imread("C:/Users/JayJay/Desktop/OpenCV_testing/dårlig/dårlig_kød.jpg", CV_LOAD_IMAGE_COLOR);
//Mat img = imread("C:/Users/JayJay/Desktop/OpenCV_testing/dårlig/dårlig_skin.jpg", CV_LOAD_IMAGE_COLOR);
Mat img = imread("C:/Users/JayJay/Desktop/OpenCV_testing/god/god_skin.jpg", CV_LOAD_IMAGE_COLOR);
//Mat img = imread("C:/Users/JayJay/Desktop/OpenCV_testing/god/god_meat.jpg", CV_LOAD_IMAGE_COLOR);
Mat injuryTemplate = imread("C:/Users/JayJay/Desktop/OpenCV_testing/dårlig/template.png", CV_LOAD_IMAGE_COLOR);

Mat bgr[3]; // Array used for color spaces R, G and B

void threshold()
{
	// Using the red(R) color space
	Mat image = bgr[2];

	imshow("Red", image);

	// Threshold and blurring processing
	threshold(image, thres, 60, 255, 1);

	// Revert white and black
	bitwise_not(thres, thres_white);

	// Blurring to get a more smooth fillet
	medianBlur(thres_white, final, 5);
}

void whitePixels()
{
	// Outputs number of white pixels
	vector<Point> white_pixels;
	findNonZero(final, white_pixels);
	int size = white_pixels.size()/1000;
	cout << "Area of herring: " << size << endl;

	// Detecting if a herring occurs or not
	if (size < 30)
	{
		cout << "Not a herring" << endl;
	}
	else
	{
		cout << "A herring occured" << endl;
	}
}

void blobDetection()
{

	// Blob detection
	SimpleBlobDetector::Params params;

	// Filter by color, 255 for white pixels
	params.filterByColor = true;
	params.blobColor = 0;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 0.01;

	// Filter by Circularity
	params.filterByCircularity = false;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.01;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01;

	Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
	vector<KeyPoint> keypoints;
	detector->detect(final, keypoints);

	// Choose color of circle (BGR)
	drawKeypoints(final, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	// Show blobs
	imshow("keypoints", im_with_keypoints);
}

void edgeDetection()
{
	// Canny Edge Detection. Use blue = 9, to remove marks inside fish
	Canny(final, canny, 50, 150, 3);

	imshow("Canny Edge Detection", canny);
}

void floodFill()
{
	// ALL BELOW THIS IS TO REMOVE BLOBS/SPOTS INSIDE THE EDGES
	copyMakeBorder(canny, canny, 1, 1, 1, 1, BORDER_REPLICATE);

	// Fill process 
	Point seed(0, 0);
	uchar fillValue = 200;
	floodFill(final, canny, seed, Scalar(255), 0, Scalar(), Scalar(), 4 | FLOODFILL_MASK_ONLY | (fillValue << 8));
	imshow("Flood fill", canny);
}

void spotFill()
{
	// Used to fill spots inside edge-detection
	// Stackoverflow c-change-color-in-cvmat-with-setto
	Mat mask;
	inRange(canny, 255, 255, mask);
	canny.setTo(0, mask);
	imshow("Spot filled", canny);
}

void skinDetection()
{
	// Using the red(R) color space
	Mat image_blue = bgr[0];

	// Threshold and blurring processing
	threshold(image_blue, impurity_thres_b, 45, 255, 1);

	// Blurring to get a more smooth fillet
	medianBlur(impurity_thres_b, impurity_final, 5);

	
	// Outputs number of white pixels
	vector<Point> white_pixels;
	findNonZero(impurity_final, white_pixels);
	cout << "Size of remaining skin: " << white_pixels.size() << " pixels" << endl << endl;

	// Detecting if a herring occurs or not
	if (white_pixels.size() > 500 && white_pixels.size() < 10000)
	{
		cout << "EXCLUDE! Remaining skin on fillet." << endl;
	}
	else
	{
		cout << "No remaining skin on fillet." << endl;
	}
	
	imshow("Original image", img);
	imshow("Skin detection", impurity_final);
}

void visibleInjuries()
{
	injuries_green = bgr[1];
	injuries_red = bgr[2];

	threshold(injuries_green, injuries_thres_g, 110, 255, 1);
	medianBlur(injuries_thres_g, injuries_g_final, 5);

	threshold(injuries_red, injuries_thres_r, 110, 255, 1);
	medianBlur(injuries_thres_r, injuries_r_final, 5);

	imshow("Green", injuries_g_final);
	imshow("Red", injuries_r_final);
}

void Indentures()
{
		// https://docs.opencv.org/3.2.0/de/da9/tutorial_template_matching.html

	Mat result;
	// Kan også skrive tal istedet for til sidst
	matchTemplate(img, injuryTemplate, result, CV_TM_CCOEFF_NORMED);
	

	imshow("Original", img);
	imshow("Template", result);
}

int main()
{
	cvtColor(img, img_grey, CV_BGR2GRAY); // Convert input image to grayscale
	split(img, bgr); //split into R, G and B color spaces

	threshold();
	whitePixels();
	blobDetection();
	edgeDetection();
	floodFill();
	spotFill();
	skinDetection();
	//Indentures();

	// Vil virke meget bedre med et stereo istedet for farve
	//visibleInjuries();
	
	
	// Imshow for start tests
	//imshow("Greyscale", img_grey);
	
	// Essential for the program not to close instantly
	waitKey(0);
}



