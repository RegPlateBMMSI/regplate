#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;

// szerokoœæ znaku, który bêdzie rozpoznawany przez sieæ neuronow¹
#define MAX_CHARACTER_WIDTH 8
// wysokoœæ znaku, który bêdzie rozpoznawany przez sieæ neuronow¹
#define MAX_CHARACTER_HEIGHT 12
// rozmiar (liczba pikseli) znaku, który bêdzie rozpoznawany przez sieæ neuronow¹
#define CHARACTER_SIZE (MAX_CHARACTER_HEIGHT * MAX_CHARACTER_WIDTH)


class PlateCharacter {
private:

public:
	Mat character;

	PlateCharacter(Mat);
};

