#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;

// szeroko�� znaku, kt�ry b�dzie rozpoznawany przez sie� neuronow�
#define MAX_CHARACTER_WIDTH 8
// wysoko�� znaku, kt�ry b�dzie rozpoznawany przez sie� neuronow�
#define MAX_CHARACTER_HEIGHT 12
// rozmiar (liczba pikseli) znaku, kt�ry b�dzie rozpoznawany przez sie� neuronow�
#define CHARACTER_SIZE (MAX_CHARACTER_HEIGHT * MAX_CHARACTER_WIDTH)


class PlateCharacter {
private:

public:
	Mat character;

	PlateCharacter(Mat);
};

