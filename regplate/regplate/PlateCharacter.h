#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;

#define MAX_CHARACTER_WIDTH 8
#define MAX_CHARACTER_HEIGHT 12


class PlateCharacter {
private:

public:
	Mat character;

	PlateCharacter(Mat);

	char * getPossibleCharacters() {

		}

};

