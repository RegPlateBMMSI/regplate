#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;

int main()
{
	Mat im_gray = imread("photo.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_bw = im_gray > 128;
	namedWindow("Tablica rejestracyjna", CV_WINDOW_AUTOSIZE);
	imshow("Tablica rejestracyjna", img_bw);
	waitKey();
	
	return 0;
}