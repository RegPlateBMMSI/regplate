#include "ConversionTools.h"


char possible_characters[POSSIBLE_CHARACTERS_COUNT] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};


std::string ConversionTools::getLicensePlateNumberFromPhotoFilename(char* filename) {
	int size = strlen(filename) - strlen("photos/") - strlen(".jpg");
	return std::string(filename + strlen("photos/"), size);
}

char* ConversionTools::plate_character_to_data(PlateCharacter &pc) {
	char* data = new char[CHARACTER_SIZE];
	char* ptr = data;
	for (int row = 0; row < MAX_CHARACTER_HEIGHT; ++row)
	{
		for (int column = 0; column < MAX_CHARACTER_WIDTH; ++column)
		{
			*ptr++ = static_cast<float>(pc.character.at<Vec3b>(row, column)[0]) / 255.f;
		}
	}
    return data;
}

char* ConversionTools::letter_to_data(char letter) {
	letter = toupper(letter);

	char* data = new char[sizeof(possible_characters)];
	for (int i = 0; i < sizeof(possible_characters); i++) {
		if (letter == possible_characters[i]) {
			data[i] = 1;
		} else {
			data[i] = 0;
		}
	}
	return data;
}

void ConversionTools::print_data(char* data, int data_size) {
	for (int i = 0; i < data_size; i++) {
		char symbol = data[i] + '0';
		cout << symbol << " ";
	}
	cout << endl;
}

