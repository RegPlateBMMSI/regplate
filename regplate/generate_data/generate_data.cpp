#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "fann.h"

#include "../common/ImagePreprocessing.h"
#include "../common/PlateCharacter.h"
#include "../common/ConversionTools.h"

//#define SHOW_IMAGES // je�li odkomentowane, poszczeg�lne znalezione znaki b�d� kolejno wy�wietlane
#define FAIL_QUICK  // je�li odkomentowane, program nie b�dzie generowa� danych testowych dla obrazk�w
					// na kt�rych nie znaleziono wszystkich znak�w, lub znaleziono ich za du�o

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	char *imageName = 0;

	if(argc==2) // je�li nie podano nazwy zdj�cia, zostaje przyj�ta warto�� domy�lna (przydatne w debugowaniu)
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

	// dokonuje transformacji na zdj�ciu, przygotowuj�cych do znajdowania znak�w
	sample.normalize(500, 120);	
	sample.generateHistogramY(); 
	sample.generateHistogramX(); 
	sample.findUpperBound(25,-0.1);
	sample.findLowerBound(25,-0.1);

	vector<Mat> images = sample.findLetters(); // wyszukuje znak�w na zdj�ciu

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
	// kolejno przetwarza znaki znajduj�ce si� na tablicy
	for(vector<Mat>::const_iterator item(images.begin()); item!=images.end() && i < licensePlateNumber.length(); ++item)
	{
		PlateCharacter pc(*item);

		// konwertuje przetwarzany znak i jego obrazek na dane wymagane przez FANN
		fann_type* plate_character_data = ConversionTools::plate_character_to_data(pc);
		fann_type* letter_data = ConversionTools::letter_to_data(licensePlateNumber[i]);
		// wy�wietla dane na standardowe wyj�cie (powinny zosta� przechwycone i zapisane do odpowiedniego pliku)
		ConversionTools::print_data(plate_character_data, CHARACTER_SIZE);
		ConversionTools::print_data(letter_data, POSSIBLE_CHARACTERS_COUNT);
		delete[] letter_data;
		delete[] plate_character_data;

#ifdef SHOW_IMAGES
		// wy�wietla nowe okno z liter� i czeka na naci�ni�cie znaku, aby przej�� dalej
		stringstream ss;
		ss << "litera " << i << ": " << licensePlateNumber[i];
		namedWindow(ss.str(), CV_WINDOW_AUTOSIZE);
		imshow(ss.str(), *item);
		waitKey();
#endif
		i++;
	}


	return 0;
}
