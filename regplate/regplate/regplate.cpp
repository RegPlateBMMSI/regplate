#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "ImagePreprocessing.h"
#include "PlateCharacter.h"

using namespace cv;
using namespace std;

/*
Nie obraŸcie siê za jakoœæ kodu, to dopiero pierwsze testy
sprawdzam, czy koncepcja z histogramem 1D jest u¿yteczna ~PG
*/

int main(int argc, char *argv[])
{
	ImagePreprocessing sample;

	if(argc==2)
		sample.loadImage(argv[1]);
	else
		sample.loadImage("zdjecia/esi.jpg");
	
	Mat img_gray = sample.getImage();

	sample.normalize(500, 120);	
	sample.generateHistogramY(); 
	sample.generateHistogramX(); 

	int imrb = sample.findUpperBound(20,0.09);
	int imrt = sample.findLowerBound(20,0.09);

	//sample.cropToBounds();
	map<int,Mat> images = sample.findLetters();
	Mat img_bw = sample.getImage();

	//cvtColor(img_bw,img_bw,CV_GRAY2BGR);
	//rectangle(img_bw,Point(0,imrt),Point(img_bw.cols,imrb),CV_RGB(255,0,0));

	namedWindow("Tablica rejestracyjna", CV_WINDOW_AUTOSIZE);
	imshow("Tablica rejestracyjna", img_gray);

	cout << "\n\n\N A C I S N I J   D O W O L N Y   K L A W I S Z\n\n" << endl;

	waitKey();

	int i=0;
	for(map<int,Mat>::const_iterator item(images.begin()); item!=images.end(); ++item)
	{
		i++;

		Mat img = (*item).second;

		PlateCharacter pc(img);

		stringstream ss;
		ss << "litera " << i;
		namedWindow(ss.str(), CV_WINDOW_AUTOSIZE);
		for (int row = 0; row < pc.character.size().height; ++row)
		{
			for (int column = 0; column < pc.character.size().width; ++column)
			{
				cout << (static_cast<float>(pc.character.at<Vec3b>(row, column)[0]) / 255.f) << " ";
			}
			cout << endl;
		}
		cout << endl;
		imshow(ss.str(), img);
		waitKey();
	}

	return 0;
}