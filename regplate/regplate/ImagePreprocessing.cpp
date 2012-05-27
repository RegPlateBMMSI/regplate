#include "ImagePreprocessing.h"


ImagePreprocessing::ImagePreprocessing(void)
{
}


ImagePreprocessing::~ImagePreprocessing(void)
{
}


/**
 * Konstruktor wczytuj¹cy obrazek z dysku
 */
ImagePreprocessing::ImagePreprocessing(string path)
{
	loadImage(path);
}


/**
 * Wczytanie pliku z dysku z automatyczn¹ konwersj¹ do skali szaroœci
 */
bool ImagePreprocessing::loadImage(string path)
{
	image = imread(path,CV_LOAD_IMAGE_GRAYSCALE);
	cout << "Wczytano plik " << path << endl;
	return true;
}


/**
 * Zwrócenie obrazka
 */
Mat ImagePreprocessing::getImage()
{
	return image;
}


/**
 * Zmiana rozmiaru oraz progowanie do czerni i bieli
 *
 * @param newWidth		nowy rozmiar obrazka
 * @param thresholdVal	próg konwersji do cz-b
 */
void ImagePreprocessing::normalize(int newWidth, int thresholdVal)
{
	resize(image, image, Size(newWidth,((float)newWidth/image.cols)*image.rows), 0, 0, INTER_NEAREST);
	cout << "Zmieniono rozmiar obrazka na " << image.cols << " x " << image.rows << endl;
	
	//image = image > treshold;
	threshold( image, image, thresholdVal, 255, 1 );
	cout << "Sprogowano obrazek do czerni i bieli z progiem " << thresholdVal << endl;
}


/**
 * Policzenie czarnych pikseli w ka¿dym z wierszy
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
 * Policzenie czarnych pikseli w ka¿dej z kolumn
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
 * Znalezienie górnej granicy tekstu
 *
 * Funkcja wyszukuje granice tekstu, znajduj¹c najwiêksz¹ ró¿nicê 
 * œredniej jasnoœci dwóch wierszy, nie wiêksz¹ ni¿ zadany próg
 * przesuwaj¹c siê w górê od œrodka obrazu (przesuniêtego o offset)
 *
 * @param offset		jak daleko od œrodka obrazu rozpocz¹æ poszukiwanie
 * @param thresholdVal	próg skoku jasnoœci
 */
int ImagePreprocessing::findUpperBound(int offset, float thresholdVal)//20,-0.1
{
	min_row_bottom=320000;

	cout << "Szukam gornej granicy" << endl;
	for(int i=image.rows/2-offset; i>1; i--) {

		if(min_row_bottom>hist_row[i]-hist_row[i-1]) {
			//próg bia³e-czarne
			if((float)min_row_bottom/image.cols>-thresholdVal)
			{
				min_row_bottom = hist_row[i]-hist_row[i-1];
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
 * Funkcja wyszukuje granice tekstu, znajduj¹c najwiêksz¹ ró¿nicê 
 * œredniej jasnoœci dwóch wierszy, nie wiêksz¹ ni¿ zadany próg
 * przesuwaj¹c siê w dó³ od œrodka obrazu (przesuniêtego o offset)
 *
 * @param offset		jak daleko od œrodka obrazu rozpocz¹æ poszukiwanie
 * @param thresholdVal	próg skoku jasnoœci
 */
int ImagePreprocessing::findLowerBound(int offset, float thresholdVal)//20,-0.1
{
	min_row_top=320000;

	cout << "Szukam dolnej granicy" << endl;
	for(int i=image.rows/2+offset; i<image.rows; i++) {

		if(min_row_top>hist_row[i-1]-hist_row[i]) {
			cout << " -> skok: " << (float)min_row_top/image.cols << " wys:" << i << endl;
			//próg bia³e-czarne
			if((float)min_row_top/image.cols>-thresholdVal)
			{
				min_row_top = hist_row[i-1]-hist_row[i];
				imrt = i;
			}
		}
	}
	imrt+=2;

	cout << "Dolna granica tekstu: " << imrt << endl;
	return imrt;
}


/**
 * Przyciêcie obrazka do wyznaczonych wczeœniej granic
 */
void ImagePreprocessing::cropToBounds()
{
	image = image(Rect(Point(0,imrt),Point(image.cols,imrb)));
}


/**
 * Wyszukiwanie obszarów spójnych
 */
void ImagePreprocessing::findLetters()
{
	Mat src=image;
	Mat dst=Mat::zeros(src.rows, src.cols, CV_8UC3);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	findContours(src, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	
	int idx=0;
	for(; idx>=0; idx=hierarchy[idx][0])
	{
		Scalar color(rand()&255, rand()&255, rand()&255);
		Rect brect = boundingRect(contours[idx]);
		
		if((brect.height>0.8*(imrt-imrb)) && (brect.height>brect.width))
		{
			drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
			rectangle(dst,brect,CV_RGB(255,0,0));
		}
	}
	
	
	image = dst;
	cropToBounds();
}