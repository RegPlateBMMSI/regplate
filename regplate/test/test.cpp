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

	if(argc==2)
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

	sample.normalize(500, 120);	
	sample.generateHistogramY(); 
	sample.generateHistogramX(); 
	sample.findUpperBound(25,-0.1);
	sample.findLowerBound(25,-0.1);

	vector<Mat> images = sample.findLetters();

	cout << "Expected letters: \t" << licensePlateNumber.length() << endl;
	cout << "Found letters: \t" << images.size() << endl;

	if (!ConversionTools::checkFoundCharactersCount(images, licensePlateNumber)) {
		cout << "Processing finished" << endl;
		exit(1);
	}
	struct fann *ann = fann_create_from_file("regplate.net");

#ifdef SHOW_IMAGES
	cout << "\n\n\N A C I S N I J   D O W O L N Y   K L A W I S Z\n\n" << endl;
	waitKey();
#endif

	int i=0;
	cout.precision(3);
	cout.setf(ios::fixed,ios::floatfield);
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
#ifdef PRINT_IMAGES
		ConversionTools::print_letter_ascii(pc);
#endif
		fann_type *output = fann_run(ann, plate_character_data);

		int best_match_index = ConversionTools::findBestMatchIndex(output);
		char letter = toupper(licensePlateNumber[i]);
		if (best_match_index >= 0) {
			cout << "Best match for letter " << letter;
			cout << " is " << ConversionTools::codeToCharacter(best_match_index);
			cout << " with propability " << output[best_match_index] << endl;
		} else {
			cout << "No match for letter " << letter << endl;
		}
		delete[] letter_data;
		delete[] plate_character_data;
		waitKey();
		i++;
	}
	
	cout << "Processing finished" << endl;
	fann_destroy(ann);

	return 0;
}
