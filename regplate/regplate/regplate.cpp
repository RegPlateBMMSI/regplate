#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace cv;
using namespace std;

/*
Nie obraŸcie siê za jakoœæ kodu, to dopiero pierwsze testy
sprawdzam, czy koncepcja z histogramem 1D jest u¿yteczna ~PG
*/

int main(int argc, char *argv[])
{
	Mat im_gray;
	if(argc==2)
		im_gray = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
	else
		im_gray = imread("photo.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_bw = im_gray > 160;

	vector<int> hist_row(im_gray.rows); 
	vector<int> hist_col(im_gray.cols); 
	int min_row_top=500, min_row_bottom=500, min_col_left, min_col_right;
	int imrt, imrb, imcl, imcr;

	// policz piksele w wierszach
	for(int i=0; i<img_bw.rows; i++){
		hist_row[i] = img_bw.cols-countNonZero(img_bw.row(i));
	}
	// policz piksele w kolumnach
	for(int i=0; i<img_bw.cols; i++){
		hist_col[i] = img_bw.rows-countNonZero(img_bw.col(i));
	}


	
	/*
	wykrywanie obszarów ju¿ dzia³a, ale próg bia³e-czarne jest dobrany eksperymentalnie
	
	UWAGA na litery typu P - tutaj skok jest wy¿szy w œrodku litery
	rozwi¹zanie	rozpoczynanie poszukiwania od pewnego momentu (tutaj eksperymentalnie +/-20px
				daje dobry efekt, ale lepsza by³aby jakaœ wartoœæ procentowa 
	*/

	//znajdŸ minimalne wartoœci poruszaj¹c siê w górê i w dó³
	for(int i=img_bw.rows/2+20; i<img_bw.rows; i++) {

		if(min_row_top>hist_row[i]-hist_row[i-1]) {
			cout << (float)min_row_top/img_bw.cols << endl;
			//próg bia³e-czarne
			if((float)min_row_top/img_bw.cols>-0.08)
			{
				min_row_top = hist_row[i]-hist_row[i-1];
				imrt = i;
			}
		}
	}
	cout << "b" << endl;
	for(int i=img_bw.rows/2-20; i>1; i--) {

		if(min_row_bottom>hist_row[i-1]-hist_row[i]) {
			cout << (float)min_row_bottom/img_bw.cols << endl;
			//próg bia³e-czarne
			if((float)min_row_bottom/img_bw.cols>-0.15)
			{
				min_row_bottom = hist_row[i-1]-hist_row[i];
				imrb = i;
			}
		}
	}

	cout << imrt << "--" << imrb << endl;

	//Mat row = img_bw.row(50);

	cvtColor(img_bw,img_bw,CV_GRAY2BGR);

	rectangle(img_bw,Point(0,imrt),Point(img_bw.cols,imrb),CV_RGB(255,0,0));

	namedWindow("Tablica rejestracyjna", CV_WINDOW_AUTOSIZE);
	imshow("Tablica rejestracyjna", img_bw);
	waitKey();
	
	return 0;
}