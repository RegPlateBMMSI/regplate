#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;

class ImagePreprocessing
{
private:
	Mat image;
	int width, height;
	vector<int> hist_row; 
	vector<int> hist_col; 
	int min_row_top, min_row_bottom;
	int imrt, imrb;
public:
	ImagePreprocessing(void);
	~ImagePreprocessing(void);
	ImagePreprocessing(string);
	bool loadImage(string);
	Mat getImage();

	void normalize(int width, int treshold);
	vector<int> generateHistogramX();
	vector<int> generateHistogramY();

	int findUpperBound(int offset, float treshold);
	int findLowerBound(int offset, float treshold);

	void cropToBounds();

	map<int,Mat> findLetters();
};

