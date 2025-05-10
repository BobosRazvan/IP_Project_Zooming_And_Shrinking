// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <algorithm>  


wchar_t* projectPath;

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar neg = 255 - val;
				dst.at<uchar>(i, j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar* lpSrc = src.data;
		uchar* lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i * w + j];
				lpDst[i * w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (r + g + b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, dst, gauss;
		src = imread(fname, IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int)k * pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss, dst, pL, pH, 3);
		imshow("input image", src);
		imshow("canny", dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame, edges, 40, 100, 3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;

		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115) { //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess)
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}




float bicubic(float s, float a)
{
	if (abs(s) <= 1) {
		return (a + 2) * pow(abs(s), 3) - (a + 3) * pow(abs(s), 2) + 1;
	}
	else if (abs(s) > 1 && abs(s) <= 2) {
		return a * pow(abs(s), 3) - 5 * a * pow(abs(s), 2) + 8 * a * abs(s) - 4 * a;
	}
	return 0;
}


void zooming_and_shrinking()
{

	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount();

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;

		std::cout << "Original image size: " << src.cols << "x" << src.rows << std::endl;
		std::cout << "Enter scale factor: " << std::endl;
		double scale;
		std::cin >> scale;
		int new_height = (int)(height * scale);
		int new_width = (int)(width * scale);
		std::cout << "Resized image size: " << new_width << "x" << new_height << std::endl;


		/// Nearest neighbor interpolation
		Mat dst1 = Mat(new_height, new_width, CV_8UC1);
		for (int i = 0; i < new_height; i++)
		{
			for (int j = 0; j < new_width; j++)
			{
				// Get the corresponding pixel in the original image
				int x = (int)(i / scale);
				int y = (int)(j / scale);
				if (x >= height) x = height - 1;
				if (y >= width) y = width - 1;

				// Assign the pixel value to the new image
				dst1.at<uchar>(i, j) = src.at<uchar>(x, y);
			}
		}



		/// Bilinear interpolation
		Mat dst2 = Mat(new_height, new_width, CV_8UC1);
		for (int i = 0; i < new_height; i++)
		{
			for (int j = 0; j < new_width; j++)
			{
				// Get the corresponding pixel in the original image
				double x = i / scale;
				double y = j / scale;

				// Get the four surrounding pixels
				int x1 = (int)x;
				int y1 = (int)y;
				int x2 = x1 + 1;
				int y2 = y1 + 1;

				if (x2 >= height) x2 = height - 1;
				if (y2 >= width) y2 = width - 1;

				int Q11 = src.at<uchar>(x1, y1);
				int Q12 = src.at<uchar>(x1, y2);
				int Q21 = src.at<uchar>(x2, y1);
				int Q22 = src.at<uchar>(x2, y2);


				///interpolate in X-Direction
				float R1, R2;
				R1 = (float)(Q11 * (x2 - x) + Q21 * (x - x1));
				R2 = (float)(Q12 * (x2 - x) + Q22 * (x - x1));

				///interpolate in Y-Direction
				dst2.at<uchar>(i, j) = (uchar)(R1 * (y2 - y) + R2 * (y - y1));

			}
		}


		///Bicubic interpolation 
		Mat dst3 = Mat(new_height, new_width, CV_8UC1);
		for (int i = 0; i < new_height; i++)
		{
			for (int j = 0; j < new_width; j++)
			{
				// Get the corresponding pixel in the original image
				double x = i / scale;
				double y = j / scale;

				/// a 4 x 4 pixel area around the pixel (x,y)
				int x1 = (int)x - 1;
				int x2 = (int)x;
				int x3 = (int)x + 1;
				int x4 = (int)x + 2;

				int y1 = (int)y - 1;
				int y2 = (int)y;
				int y3 = (int)y + 1;
				int y4 = (int)y + 2;

				if (x1 < 0) x1 = 0;
				if (x2 < 0) x2 = 0;
				if (x3 >= height) x3 = height - 1;
				if (x4 >= height) x4 = height - 1;

				if (y1 < 0) y1 = 0;
				if (y2 < 0) y2 = 0;
				if (y3 >= width) y3 = width - 1;
				if (y4 >= width) y4 = width - 1;



				///Compute the coeficients (a)
				double sx1 = x - x1;
				double sx2 = x - x2;
				double sx3 = x - x3;
				double sx4 = x - x4;

				double sy1 = y - y1;
				double sy2 = y - y2;
				double sy3 = y - y3;
				double sy4 = y - y4;

				// bicubic coefficient used in Adobe Photoshop
				float a = -0.5f;

				// bicubic coefficient used in X-Interpolation
				float wx1 = bicubic(sx1, a);
				float wx2 = bicubic(sx2, a);
				float wx3 = bicubic(sx3, a);
				float wx4 = bicubic(sx4, a);

				float wy1 = bicubic(sy1, a);
				float wy2 = bicubic(sy2, a);
				float wy3 = bicubic(sy3, a);
				float wy4 = bicubic(sy4, a);


				// Get the 16 surrounding pixels
				int Q11 = src.at<uchar>(x1, y1);
				int Q12 = src.at<uchar>(x1, y2);
				int Q13 = src.at<uchar>(x1, y3);
				int Q14 = src.at<uchar>(x1, y4);
				int Q21 = src.at<uchar>(x2, y1);
				int Q22 = src.at<uchar>(x2, y2);
				int Q23 = src.at<uchar>(x2, y3);
				int Q24 = src.at<uchar>(x2, y4);
				int Q31 = src.at<uchar>(x3, y1);
				int Q32 = src.at<uchar>(x3, y2);
				int Q33 = src.at<uchar>(x3, y3);
				int Q34 = src.at<uchar>(x3, y4);
				int Q41 = src.at<uchar>(x4, y1);
				int Q42 = src.at<uchar>(x4, y2);
				int Q43 = src.at<uchar>(x4, y3);
				int Q44 = src.at<uchar>(x4, y4);

				// Interpolate in X - Direction

				float R1 = wx1 * (Q11 * wy1 + Q12 * wy2 + Q13 * wy3 + Q14 * wy4);
				float R2 = wx2 * (Q21 * wy1 + Q22 * wy2 + Q23 * wy3 + Q24 * wy4);
				float R3 = wx3 * (Q31 * wy1 + Q32 * wy2 + Q33 * wy3 + Q34 * wy4);
				float R4 = wx4 * (Q41 * wy1 + Q42 * wy2 + Q43 * wy3 + Q44 * wy4);

				// Interpolate in Y - Direction
				float result = R1 + R2 + R3 + R4;
				result = min(max(result, 0.0f), 255.0f);
				dst3.at<uchar>(i, j) = static_cast<uchar>(result);


			}
		}



		imshow("Input image", src);
		imshow("Resized image (nearest neighbour)", dst1);
		imshow("Resized image (bilinear)", dst2);
		imshow("Resized image (bicubic)", dst3);

		waitKey();
	}
}





int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Image negative\n");
		printf(" 4 - Image negative (fast)\n");
		printf(" 5 - BGR->Gray\n");
		printf(" 6 - BGR->Gray (fast, save result to disk) \n");
		printf(" 7 - BGR->HSV\n");
		printf(" 8 - Resize image\n");
		printf(" 9 - Canny edge detection\n");
		printf(" 10 - Edges in a video sequence\n");
		printf(" 11 - Snap frame from live video\n");
		printf(" 12 - Mouse callback demo\n");
		printf(" 13 - Zooming & Shrinking in Digital Images\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1:
			testOpenImage();
			break;
		case 2:
			testOpenImagesFld();
			break;
		case 3:
			testNegativeImage();
			break;
		case 4:
			testNegativeImageFast();
			break;
		case 5:
			testColor2Gray();
			break;
		case 6:
			testImageOpenAndSave();
			break;
		case 7:
			testBGR2HSV();
			break;
		case 8:
			testResize();
			break;
		case 9:
			testCanny();
			break;
		case 10:
			testVideoSequence();
			break;
		case 11:
			testSnap();
			break;
		case 12:
			testMouseClick();
			break;
		case 13:
			zooming_and_shrinking();
			break;
		}
	} while (op != 0);
	return 0;
}