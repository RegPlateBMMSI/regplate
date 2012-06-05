#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;

#define MAX_CHARACTER_WIDTH 16
#define MAX_CHARACTER_HEIGHT 16


class PlateCharacter {
private:

public:
	Mat character;

	PlateCharacter(Mat);

};
