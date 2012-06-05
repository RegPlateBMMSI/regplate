#include "PlateCharacter.h"


PlateCharacter::PlateCharacter(Mat src)
{
	float xyratio = static_cast<float>(src.size().width) / static_cast<float>(src.size().height);
	int size_width, size_height;
	if (xyratio > (MAX_CHARACTER_WIDTH / MAX_CHARACTER_HEIGHT))
	{
		size_width = MAX_CHARACTER_WIDTH;
		size_height = static_cast<int>((MAX_CHARACTER_WIDTH / static_cast<float>(src.size().width)) * src.size().height);
	} else {
		size_width = static_cast<int>((MAX_CHARACTER_HEIGHT / static_cast<float>(src.size().height)) * src.size().width);
		size_height = MAX_CHARACTER_HEIGHT;
	}

	character = Mat(MAX_CHARACTER_WIDTH, MAX_CHARACTER_HEIGHT, DataType<Vec3b>::type, Scalar(0,0,0));

	resize(src, src, Size(size_width,size_height), 0, 0, INTER_NEAREST);

	int align_center = (MAX_CHARACTER_WIDTH - size_width) / 2;
	
	for (int row = 0; row < src.size().height; ++row)
	{
		for (int column = 0; column < src.size().width; ++column)
		{
			character.at<Vec3b>(row, column + align_center) = src.at<Vec3b>(row, column);
		}
	}

}


