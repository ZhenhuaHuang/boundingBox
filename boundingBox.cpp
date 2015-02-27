
// read all pictures in a folder "inputDIR"
// find bounding rectangles for each picture
// draw rectangle on each picture and output in the folder "outputDIR"
// display picture file names and corresponding bounding rectangles


#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <dirent.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// DIR path for input pictures and output pictures
string inputDIR = "./fish1";		// input DIR path
string outputDIR = "./fish1_results";		// output DIR path


// compare the area of rectangles, to sort rectangles according to area
bool compareRect( const Rect & ls, const Rect & rs) {
	return (ls.width * ls.height) > (rs.width * rs.height);
}


// get all picture file names in the dir
int getFileNames(const string inputDIR, vector<string> &fileNames) {

	DIR *pDIR;
	dirent* pEntry;

	pDIR = opendir(inputDIR.c_str());	// open dir

	// check if dir is valid
	if( pDIR == NULL) {
		cout << "Error opening dir: " << inputDIR << endl;
		return -1;
	}

	// ignore the elements . and ..
	if( !(pEntry = readdir(pDIR))) 
		return -2;
	if( !(pEntry = readdir(pDIR))) 
		return -3;

	// read each element in DIR and store in the vector
	while( pEntry = readdir(pDIR)) {
		fileNames.push_back(pEntry->d_name);
	}

	// close dir
	closedir(pDIR);

	return 0;

}

// find bounding rectangles of objects in pictures
int findRects( const vector<string> fileNames, map<string, Rect> & mapFilesRects) {

	Mat srcImage;			// input image
	Mat grayImage;			// converted to gray
	Mat thresholdImage;		// converted to binary in range
	Mat outputImage;		// output image

	// structure element for morphology transformation
	Mat dilateElement;		
	dilateElement = getStructuringElement(MORPH_RECT, Size(5,5));

	// find bounding box for each picture in one loop
	for( int i = 0; i < fileNames.size(); ++i) {
		
		// read image
		string inputFilePath = inputDIR + "/" + fileNames.at(i);
		srcImage = imread(inputFilePath);
		srcImage.copyTo(outputImage);

		// convert image
		cvtColor(srcImage, grayImage, CV_BGR2GRAY);
		inRange(grayImage, 0, 240, thresholdImage);

		// morphology transformation, = close here
		dilate(thresholdImage, thresholdImage, dilateElement);
		erode(thresholdImage, thresholdImage, dilateElement);

		// find contours
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(thresholdImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		// find bounding boxes
		vector<Rect> boundingBoxes;
		for( int i = 0; i < contours.size(); ++i) {
			Rect bounding = boundingRect(contours.at(i));
			boundingBoxes.push_back(bounding);
		}

		// sort bouding boxes according to area
		sort(boundingBoxes.begin(), boundingBoxes.end(), compareRect);

		// show the box with the largest area
		Rect rectMax;
		rectMax = boundingBoxes.at(0);
		rectangle(outputImage, Point(rectMax.x, rectMax.y), Point(rectMax.x + rectMax.width, rectMax.y + rectMax.height), Scalar(0, 255, 0), 2);

		imshow("output", outputImage);
		waitKey(10);
		
		// create outputDIR if it does exist
		if( !opendir(outputDIR.c_str())) {
			CreateDirectoryA(outputDIR.c_str(), NULL);
		}

		// write results in outputDIR
		string outputFilePath = outputDIR + "/" + fileNames.at(i);
		imwrite(outputFilePath, outputImage);

		// create a map between file names and corresponding bounding rectangles
		mapFilesRects.insert(pair<string, Rect>(fileNames.at(i), rectMax));
	}

	return 0;
}


int main() {

	vector<string> fileNames;		// vector to store all file paths in the dir

	getFileNames(inputDIR, fileNames);		// get all file paths in the dir

	// display picture file names
	//cout << fileNames.size() << endl;
	//for( int i = 0; i < fileNames.size(); ++i) {
	//	cout << fileNames.at(i) << endl;
	//}

	// create map of files and rectangles
	map<string, Rect> mapFilesRects;

	// find bouding boxes
	findRects(fileNames, mapFilesRects);

	// display picture file names and corresponding bouding rectangles
	for(map<string, Rect>::iterator it = mapFilesRects.begin(); it != mapFilesRects.end(); ++it) {
		cout << it->first << ": " << it->second << endl;
	}

	cin.sync();
	cin.get();
	return 0;
}