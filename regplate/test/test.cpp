#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "../common/ImagePreprocessing.h"
#include "../common/PlateCharacter.h"
#include "../common/ConversionTools.h"

#include "fann.h"

//#define SHOW_IMAGES
//#define PRINT_IMAGES

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
	cout << "Processing file: \t" << licensePlateNumber << endl;

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

	// wy�wietla log o znalezionych literach
	cout << "Expected letters: \t" << licensePlateNumber.length() << endl;
	cout << "Found letters: \t" << images.size() << endl;

	if (!ConversionTools::checkFoundCharactersCount(images, licensePlateNumber)) {
		cout << "Processing finished" << endl; // ko�czy dzia�anie, nie znaleziono wszystkich znak�w, lub jest ich za du�o
		exit(1);
	}

	// odczytuje sie� neuronow� z pliku
	struct fann *ann = fann_create_from_file("regplate.net");

#ifdef SHOW_IMAGES
	cout << "\n\n\N A C I S N I J   D O W O L N Y   K L A W I S Z\n\n" << endl;
	waitKey();
#endif

	// ustawia parametry wypisywania liczb zmiennoprzecinkowych dla log�w
	int i=0;
	cout.precision(3);
	cout.setf(ios::fixed,ios::floatfield);

	// kolejno przetwarza znaki znajduj�ce si� na tablicy
	for(vector<Mat>::const_iterator item(images.begin()); item!=images.end() && i < licensePlateNumber.length(); ++item)
	{
		PlateCharacter pc(*item); // skaluje obrazek i zachowuje go w zmiennej pc

		// konwertuje obrazek z przetwarzanym znakiem na dane wymagane przez FANN
		fann_type* plate_character_data = ConversionTools::plate_character_to_data(pc);

		// uruchamia sie� neuronow� na danych pochodz�cych z odczytanego znaku
		fann_type *output = fann_run(ann, plate_character_data);	

		// w�r�d wynik�w znajduje znak z najwi�kszym przypisanym przez FANN prawdopodobie�stwem trafienia
		int best_match_index = ConversionTools::findBestMatchIndex(output);
		char letter = toupper(licensePlateNumber[i]);
		if (best_match_index >= 0) {
			// wy�wietla informacj� o szukanym znaku, znaku rozpoznanym przez sie� i prawdopodobie�stwie,
			// �e to te same znaki
			cout << "Best match for letter " << letter;
			cout << " is " << ConversionTools::codeToCharacter(best_match_index);
			cout << " with propability " << output[best_match_index] << endl;
		} else {
			// je�li wszystkie znaki maj� prawdopodobie�stwo 0, wy�wietla komunikat o braku trafienia
			cout << "No match for letter " << letter << endl;
		}
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
	
	cout << "Processing finished" << endl;
	fann_destroy(ann);

	return 0;
}
