#include "ImagePreprocessing.h"


ImagePreprocessing::ImagePreprocessing(void)
{
}


ImagePreprocessing::~ImagePreprocessing(void)
{
}


/**
 * Konstruktor wczytuj�cy obrazek z dysku
 */
ImagePreprocessing::ImagePreprocessing(string path)
{
	loadImage(path);
}


/**
 * Wczytanie pliku z dysku z automatyczn� konwersj� do skali szaro�ci
 */
bool ImagePreprocessing::loadImage(string path)
{
	image = imread(path,CV_LOAD_IMAGE_GRAYSCALE);
	//cout << "Wczytano plik " << path << endl;
	return true;
}


/**
 * Zwr�cenie obrazka
 */
Mat ImagePreprocessing::getImage()
{
	return image;
}


/**
 * Zmiana rozmiaru oraz progowanie do czerni i bieli
 *
 * @param newWidth		nowy rozmiar obrazka
 * @param thresholdVal	pr�g konwersji do cz-b
 */
void ImagePreprocessing::normalize(int newWidth, int thresholdVal)
{
	resize(image, image, Size(newWidth,((float)newWidth/image.cols)*image.rows), 0, 0, INTER_NEAREST);
	clog << "Zmieniono rozmiar obrazka na " << image.cols << " x " << image.rows << endl;
	
	//image = image > treshold;
	threshold( image, image, thresholdVal, 255, 1 );
	clog << "Sprogowano obrazek do czerni i bieli z progiem " << thresholdVal << endl;
}


/**
 * Policzenie czarnych pikseli w ka�dym z wierszy
 */
vector<int> ImagePreprocessing::generateHistogramX()
{
	for(int i=0; i<image.cols; i++){
		hist_col.push_back(image.rows-countNonZero(image.col(i)));
	}

	//cout << "Wyznaczono histogram poziomy" << endl;
	return hist_col;
}


/**
 * Policzenie czarnych pikseli w ka�dej z kolumn
 */
vector<int> ImagePreprocessing::generateHistogramY()
{
	// policz czarne piksele w wierszach
	for(int i=0; i<image.rows; i++){
		hist_row.push_back(image.cols-countNonZero(image.row(i)));
	}

	//cout << "Wyznaczono histogram pionowy" << endl;
	return hist_row;
}


/**
 * Znalezienie g�rnej granicy tekstu
 *
 * Funkcja wyszukuje granice tekstu, znajduj�c najwi�ksz� r�nic� 
 * �redniej jasno�ci dw�ch wierszy, nie wi�ksz� ni� zadany pr�g
 * przesuwaj�c si� w g�r� od �rodka obrazu (przesuni�tego o offset)
 *
 * @param offset		jak daleko od �rodka obrazu rozpocz�� poszukiwanie
 * @param thresholdVal	pr�g skoku jasno�ci
 */	
int ImagePreprocessing::findUpperBound(int offset, float thresholdVal)//20,-0.1
{
	min_row_bottom=320000;

	clog << "Szukam gornej granicy" << endl;
	for(int i=image.rows/2-offset; i>1; i--) {

		if(min_row_bottom>hist_row[i]-hist_row[i-1]) {
			//pr�g bia�e-czarne
			if((float)min_row_bottom/image.cols>-thresholdVal)
			{
				min_row_bottom = hist_row[i]-hist_row[i-1];
				imrb = i;
				clog << " -> skok: " << (float)min_row_bottom/image.cols << " wys:" << i << endl;
			}
			else
				break;
		}
	}
	imrb-=2;

	clog << "Gorna granica tekstu: " << imrb << endl;
	return imrb;
}


/**
 * Znalezienie dolnej granicy tekstu
 *
 * Funkcja wyszukuje granice tekstu, znajduj�c najwi�ksz� r�nic� 
 * �redniej jasno�ci dw�ch wierszy, nie wi�ksz� ni� zadany pr�g
 * przesuwaj�c si� w d� od �rodka obrazu (przesuni�tego o offset)
 *
 * @param offset		jak daleko od �rodka obrazu rozpocz�� poszukiwanie
 * @param thresholdVal	pr�g skoku jasno�ci
 */
int ImagePreprocessing::findLowerBound(int offset, float thresholdVal)//20,-0.1
{
	min_row_top=320000;

	clog << "Szukam dolnej granicy" << endl;
	for(int i=image.rows/2+offset; i<image.rows; i++) {

		if(min_row_top>hist_row[i-1]-hist_row[i]) {
			clog << " -> skok: " << (float)min_row_top/image.cols << " wys:" << i << endl;
			//pr�g bia�e-czarne
			if((float)min_row_top/image.cols>-thresholdVal)
			{
				min_row_top = hist_row[i-1]-hist_row[i];
				imrt = i;
			}
		}
	}
	imrt+=2;

	clog << "Dolna granica tekstu: " << imrt << endl;
	return imrt;
}


/**
 * Przyci�cie obrazka do wyznaczonych wcze�niej granic
 */
void ImagePreprocessing::cropToBounds()
{
	image = image(Rect(Point(0,imrt),Point(image.cols,imrb)));
}


/**
 * Wyszukiwanie liter na obrazku
 *
 * @return	Mapa <offset x litery, obrazek z liter�>
 */
vector<Mat> ImagePreprocessing::findLetters()
{
	Mat src=image;
	//Mat dst=Mat::zeros(src.rows, src.cols, CV_8UC3);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	map<int,Mat> images;
	
	findContours(src, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	int heightSum = 0;
	int lettersCount = 0;
	
	for(int idx=0; idx>=0; idx=hierarchy[idx][0])
	{
		//Scalar color(rand()&255, rand()&255, rand()&255);
		Rect brect = boundingRect(contours[idx]);
		
		if((brect.height>0.6*(imrt-imrb)) && (brect.height>brect.width))
		{
			Mat letter=Mat::zeros(src.rows, src.cols, CV_8UC3);
			drawContours(letter, contours, idx, CV_RGB(255,255,255), CV_FILLED, 8, hierarchy);
			letter=letter(brect);

			heightSum += letter.size().height;
			lettersCount++;

			images[brect.x]=letter;
		}
	}

	vector<Mat> sortedImages;
	for(map<int,Mat>::const_iterator item(images.begin()); item!=images.end(); ++item) {
		sortedImages.push_back((*item).second);
	}

	return sortedImages;
}