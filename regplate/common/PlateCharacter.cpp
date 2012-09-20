#include "PlateCharacter.h"


PlateCharacter::PlateCharacter(Mat src)
{
	int size_width, size_height;
	float xyratio = static_cast<float>(src.size().width) / static_cast<float>(src.size().height);
	size_height = MAX_CHARACTER_HEIGHT;
	if (xyratio > (MAX_CHARACTER_WIDTH / static_cast<float>(MAX_CHARACTER_HEIGHT))) {
		size_width = MAX_CHARACTER_WIDTH;
	} else {
		size_width = ceil(MAX_CHARACTER_HEIGHT / static_cast<float>(src.size().height) * src.size().width);
	}

	character = Mat(MAX_CHARACTER_HEIGHT, MAX_CHARACTER_WIDTH, DataType<Vec3b>::type, Scalar(0,0,0));

	resize(src, src, Size(size_width,size_height), 0, 0, INTER_NEAREST);

	int align_center = (MAX_CHARACTER_WIDTH - size_width) / 2;

	int fii = character.size().width;
	int foo = character.size().height;
	
	for (int row = 0; row < src.size().height; ++row)
	{
		for (int column = 0; column < src.size().width; ++column)
		{
			Vec3b a = src.at<Vec3b>(row, column);
			character.at<Vec3b>(row, column + align_center) = a;
		}
	}

}


