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
	cout << "Wczytano plik " << path << endl;
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
 * @param newWidth	nowy rozmiar obrazka
 * @param treshold	pr�g konwersji do cz-b
 */
void ImagePreprocessing::normalize(int newWidth, int treshold)
{
	resize(image, image, Size(newWidth,((float)newWidth/image.cols)*image.rows), 0, 0, INTER_NEAREST);
	cout << "Zmieniono rozmiar obrazka na " << image.cols << " x " << image.rows << endl;
	
	image = image > treshold;
	cout << "Sprogowano obrazek do czerni i bieli z progiem " << treshold << endl;
}


/**
 * Policzenie czarnych pikseli w ka�dym z wierszy
 */
vector<int> ImagePreprocessing::generateHistogramX()
{
	for(int i=0; i<image.cols; i++){
		hist_col.push_back(image.rows-countNonZero(image.col(i)));
	}

	cout << "Wyznaczono histogram poziomy" << endl;
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

	cout << "Wyznaczono histogram pionowy" << endl;
	return hist_row;
}


/**
 * Znalezienie g�rnej granicy tekstu
 *
 * Funkcja wyszukuje granice tekstu, znajduj�c najwi�ksz� r�nic� 
 * �redniej jasno�ci dw�ch wierszy, nie wi�ksz� ni� zadany pr�g
 * przesuwaj�c si� w g�r� od �rodka obrazu (przesuni�tego o offset)
 *
 * @param offset	jak daleko od �rodka obrazu rozpocz�� poszukiwanie
 * @param treshold	pr�g skoku jasno�ci
 */
int ImagePreprocessing::findUpperBound(int offset, float treshold)//20,-0.1
{
	min_row_bottom=320000;

	cout << "Szukam gornej granicy" << endl;
	for(int i=image.rows/2-offset; i>1; i--) {

		if(min_row_bottom>hist_row[i-1]-hist_row[i]) {
			//pr�g bia�e-czarne
			if((float)min_row_bottom/image.cols>-treshold)
			{
				min_row_bottom = hist_row[i-1]-hist_row[i];
				imrb = i;
				cout << " -> skok: " << (float)min_row_bottom/image.cols << " wys:" << i << endl;
			}
			else
				break;
		}
	}
	imrb-=2;

	cout << "Gorna granica tekstu: " << imrb << endl;
	return imrb;
}


/**
 * Znalezienie dolnej granicy tekstu
 *
 * Funkcja wyszukuje granice tekstu, znajduj�c najwi�ksz� r�nic� 
 * �redniej jasno�ci dw�ch wierszy, nie wi�ksz� ni� zadany pr�g
 * przesuwaj�c si� w d� od �rodka obrazu (przesuni�tego o offset)
 *
 * @param offset	jak daleko od �rodka obrazu rozpocz�� poszukiwanie
 * @param treshold	pr�g skoku jasno�ci
 */
int ImagePreprocessing::findLowerBound(int offset, float treshold)//20,-0.1
{
	min_row_top=320000;

	cout << "Szukam dolnej granicy" << endl;
	for(int i=image.rows/2+offset; i<image.rows; i++) {

		if(min_row_top>hist_row[i]-hist_row[i-1]) {
			cout << " -> skok: " << (float)min_row_top/image.cols << " wys:" << i << endl;
			//pr�g bia�e-czarne
			if((float)min_row_top/image.cols>-treshold)
			{
				min_row_top = hist_row[i]-hist_row[i-1];
				imrt = i;
			}
		}
	}
	imrt+=2;

	cout << "Dolna granica tekstu: " << imrt << endl;
	return imrt;
}


/**
 * Test
 */
void ImagePreprocessing::findLetters()
{
	//findContours(image,
}