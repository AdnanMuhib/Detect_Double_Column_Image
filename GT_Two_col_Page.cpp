// standard Libraries
#include <iostream>
#include <fstream>
#include<conio.h>
#include <string>
#include<vector>

// openCV Libraries
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include <opencv2\opencv.hpp>

// directory manipulating Libraries
#include "dirent.h"
#include <sys/stat.h>
#include <sys/types.h>

// namespaces
using namespace std;
using namespace cv;

// function declaration
int ground_Truth(string,string,int,int,int);

// main Function
int main()
{
	ifstream fin;
	string dir, filepath,fileName;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	cout << "Enter Folder Path : " << flush;
	getline(cin, dir);  // gets everything the user ENTERs

	dp = opendir(dir.c_str());
	if (dp == NULL)
	{
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}

	while ((dirp = readdir(dp)))
	{
		filepath = dir + "/" + dirp->d_name;
		
		// If the file is a directory (or is in some way invalid) we'll skip it 
		if (stat(filepath.c_str(), &filestat)) continue;
		if (S_ISDIR(filestat.st_mode))         continue;
		
		// getting fileName Without Extension
		fileName = "";
		for (int i = 0; i < strlen(dirp->d_name); i++)
		{
			if (dirp->d_name[i]=='.')
			{
				break;
			}
			fileName += dirp->d_name[i];
		}
		
		cout << "\t" << fileName << endl;
		
		// converting  backslash to forward slash
		for(int i=0; i<filepath.length(); i++)
		{
			if (filepath[i] == '\\')
			{
				filepath[i] = '/';
			}
		}
		// passing file to ground truth function
		ground_Truth(filepath, fileName,340,500,30);
	}
	closedir(dp);
	system("pause");
	return 0;
}

// function defination

int ground_Truth(string imgpath, //complete path
	string fileName,			// output file name
	int leftMargin=340,			// left and right margin of the page to ignore
	int rightMargin=500,int pixToIgnore=30) // number of pixles in a column to ignore
	{
		fileName += "_gt.txt";
		cout << "\n\t " << fileName;
		// converting imgPath to char pointer 

		char *path = new char[imgpath.length() + 1];
		path[imgpath.length()] = '\0';
		for (int i = 0; i < imgpath.length(); i++)
		{
			path[i] = imgpath[i];
		}

		// converting output File name into char array 

		char *fname = new char[fileName.length() + 1];
		fname[fileName.length()] = '\0';
		for (int i = 0; i < fileName.length(); i++)
		{
			fname[i] = fileName[i];
		}

		// loading image
		IplImage *img = cvLoadImage(path);
		// get image size
		CvSize imgSize = cvGetSize(img);
		Mat image = img;
		// Gray scale image
		IplImage *gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
		cvCvtColor(img, gray, CV_RGB2GRAY);

		// binarized image
		IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
		cvThreshold(gray, binary, 5, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

		double pixel;
		int count = 0;
		int height = binary->height;
		int *linecount = new int[height];
		int width = binary->width;
		int *wordcount = new int[width];
		// Array to store the black pixel of Vertical Histogram
		// Dynamically Allocating memory 

		int *VerticalHistogram = new int[width];

		// initializing the array with 0 

		for (int i = 0; i < width; i++)
		{
			VerticalHistogram[i] = 0;
		}

		// Assigning the values for black pixels and printing 
		//printf("\t\tVertical Bin Values \n");

		int colValue = 0;
		int aPoint = 0,bPoint=0;
	
		// flag for the a and b point detection of column line in page
		bool flag = TRUE; 

		for (int j = 0; j < (binary->width); j++)
		{
			count = 0;
			for (int i = 0; i < (binary->height); i++) 
			{
				pixel = cvGetReal2D(binary, i, j);
				if (pixel == 0)
				{
					VerticalHistogram[j]++;
					count++;
				}
			}
		
			// if pixel found in a column are too less then they will be ignored
			if (count<pixToIgnore)
			{
				count = 0;
			}
			if (j > leftMargin && j < width - rightMargin && count == 0)
			{
				if (flag)
				{
					aPoint = j;
					flag = FALSE;
				}
				bPoint = j;
			}
		}
		colValue = (bPoint + aPoint) / 2;
	
		cout <<"\tColumn at :  "<< colValue<<endl;
	
		// croping image and displaying 
		int offset_x = colValue;
		int offset_y = 0;
	
	
		// crop values for the left half of the picture
		Rect left;
		left.x = 0;
		left.y = 0;
		left.width = colValue;
		left.height = image.size().height - (offset_y * 2);

		// cooridnates of left side picture
		int x0 = left.x;
		int x1 = left.width;
		int y0 = left.y;
		int y1 = left.height;

	
		// writing coorinates of left side to a  file
		ofstream gt_txt(fname);

		gt_txt << x0 << " " << x1 << " " << y0 << " " << y1 << endl;
		// file remains open to write the values of left image
	
		// Values for  Right half of image
		Rect right;
		right.x = offset_x;
		right.y = 0;
		right.width = image.size().width - (offset_x);
		right.height = image.size().height - (offset_y * 2);
	
		// cooridnates of the Right side picture
		x0 = right.x;
		x1 = right.width;
		y0 = right.y;
		y1 = right.height;
	
		// writing coordinates on pre opened file	
		gt_txt << x0 << " " << x1 << " " << y0 << " " << y1 << endl;
	
		// file closed

		gt_txt.close();
	
		// drawing recatangle on right half
		//cvRectangle(img, cvPoint(x0, y0), cvPoint(x1, y1), CV_RGB(255, 0, 0), 5, 8);
		// displaying marked image
		//cvNamedWindow("img", CV_WINDOW_NORMAL);
		//cvShowImage("img", img);
	
		/* Crop the original image to the defined ROI */

		//Mat rightHalf = image(right);
		//Mat leftHalf = image(left);
		//displaying the left and right halves of the  image
		//
		//cvNamedWindow("rightHalf", CV_WINDOW_NORMAL);
		//imshow("rightHalf", rightHalf);
		//
		//cvNamedWindow("leftHalf", CV_WINDOW_NORMAL);
		//imshow("leftHalf", leftHalf);
		//
		//// saving image right half
		//imwrite("right_half.png", rightHalf);
		//// saving left half
		//imwrite("left_half.png", leftHalf);
		//cv::waitKey(0);

		// releasing memory 
		delete[] VerticalHistogram;
		return 1;
}