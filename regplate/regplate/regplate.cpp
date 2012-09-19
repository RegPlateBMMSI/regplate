#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "ImagePreprocessing.h"
#include "PlateCharacter.h"

#define SHOW_IMAGES
#define PRINT_IMAGES
//#define FAIL_QUICK

using namespace cv;
using namespace std;

void plate_character_to_ascii(PlateCharacter &pc);
void letter_to_code(char c);

char * getLicensePlateNumberFromPhotoFilename(char* filename) {
	int size = strlen(filename) - strlen("zdjecia/") - strlen(".jpg");
	char* buff = new char[size + 1];
	buff[size] = 0;
	strncpy(buff, filename + strlen("zdjecia/"), size);
	return buff;
}

int main(int argc, char *argv[])
{
	ImagePreprocessing sample;
	char *imageName = 0;

	if(argc==2)
		imageName = argv[1];
	else
		imageName = "zdjecia/sz28243.jpg";

	sample.loadImage(imageName);

	Mat img_gray = sample.getImage();
	sample.normalize(500, 160);	
	sample.generateHistogramY(); 
	sample.generateHistogramX(); 
	sample.findUpperBound(25,-0.1);
	sample.findLowerBound(25,-0.1);

#ifdef SHOW_IMAGES
	namedWindow("Tablica rejestracyjna", CV_WINDOW_AUTOSIZE);
	imshow("Tablica rejestracyjna", img_gray);
#endif

	vector<Mat> images = sample.findLetters();
	char* licensePlateNumber = getLicensePlateNumberFromPhotoFilename(imageName);

	if (images.size() != strlen(licensePlateNumber)) {
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
	for(vector<Mat>::const_iterator item(images.begin()); item!=images.end(); ++item)
	{
		Mat img = *item;

#ifdef SHOW_IMAGES
		stringstream ss;
		ss << "litera " << i;
		namedWindow(ss.str(), CV_WINDOW_AUTOSIZE);
		imshow(ss.str(), img);
#endif

		PlateCharacter pc(img);

		plate_character_to_ascii(pc);
		letter_to_code(licensePlateNumber[i]);
		waitKey();

		i++;
	}

	delete licensePlateNumber;
	return 0;
}

void plate_character_to_ascii(PlateCharacter &pc) {
#ifdef PRINT_IMAGES
	for (int row = 0; row < pc.character.size().height; ++row)
	{
		for (int column = 0; column < pc.character.size().width; ++column)
		{
			cout << (static_cast<float>(pc.character.at<Vec3b>(row, column)[0]) / 255.f) << " ";
		}
#ifdef SHOW_IMAGES
		cout << endl;
#endif  
	}
    cout << endl;
#endif
}


char possible_characters[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};


void letter_to_code(char letter) {
#ifdef PRINT_IMAGES
	letter = toupper(letter);
	for (int i = 0; i < sizeof(possible_characters); i++) {
		if (letter == possible_characters[i]) {
			cout << "1 ";
		} else {
			cout << "0 ";
		}
	}
	cout << endl;
#endif
}
