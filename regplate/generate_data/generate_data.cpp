#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "fann.h"

#include "../common/ImagePreprocessing.h"
#include "../common/PlateCharacter.h"
#include "../common/ConversionTools.h"

//#define SHOW_IMAGES
#define FAIL_QUICK

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	char *imageName = 0;

	if(argc==2)
		imageName = argv[1];
	else
		imageName = "photos/sz28243.jpg";

	string licensePlateNumber = ConversionTools::getLicensePlateNumberFromPhotoFilename(imageName);

	ImagePreprocessing sample;
	sample.loadImage(imageName);

#ifdef SHOW_IMAGES
	namedWindow("Tablica rejestracyjna", CV_WINDOW_AUTOSIZE);
	imshow("Tablica rejestracyjna", sample.getImage());
#endif

	sample.normalize(500, 120);	
	sample.generateHistogramY(); 
	sample.generateHistogramX(); 
	sample.findUpperBound(25,-0.1);
	sample.findLowerBound(25,-0.1);

	vector<Mat> images = sample.findLetters();
	if (images.size() != licensePlateNumber.length()) {
		cerr << "Nieprawidlowa liczba znalezionych znakow (" << images.size() << ") w pliku: " << imageName << endl;
#ifdef FAIL_QUICK
		exit(1);
#endif
	}

#ifdef SHOW_IMAGES
	cout << "\n\n\N A C I S N I J   D O W O L N Y   K L A W I S Z\n\n" << endl;
	waitKey();
#endif

	int i=0;
	for(vector<Mat>::const_iterator item(images.begin()); item!=images.end() && i < licensePlateNumber.length(); ++item)
	{
#ifdef SHOW_IMAGES
		stringstream ss;
		ss << "litera " << i << ": " << licensePlateNumber[i];
		namedWindow(ss.str(), CV_WINDOW_AUTOSIZE);
		imshow(ss.str(), *item);
#endif

		PlateCharacter pc(*item);

		fann_type* plate_character_data = ConversionTools::plate_character_to_data(pc);
		fann_type* letter_data = ConversionTools::letter_to_data(licensePlateNumber[i]);
		ConversionTools::print_data(plate_character_data, CHARACTER_SIZE);
		ConversionTools::print_data(letter_data, POSSIBLE_CHARACTERS_COUNT);
		delete[] letter_data;
		delete[] plate_character_data;
		waitKey();
		i++;
	}


	return 0;
}
