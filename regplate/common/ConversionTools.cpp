#include "ConversionTools.h"


char possible_characters[POSSIBLE_CHARACTERS_COUNT] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};


std::string ConversionTools::getLicensePlateNumberFromPhotoFilename(char* filename) {
	int size = strlen(filename) - strlen("photos/") - strlen(".jpg");
	return std::string(filename + strlen("photos/"), size);
}

bool ConversionTools::checkFoundCharactersCount(std::vector<Mat> images, string licensePlateNumber) {
	if (images.size() != licensePlateNumber.length()) {
		cerr << "Nieprawidlowa liczba znalezionych znakow (" << images.size() << ") w pliku: " << licensePlateNumber << endl;
		return false;
	} else {
		return true;
	}
}

fann_type* ConversionTools::plate_character_to_data(PlateCharacter &pc) {
	fann_type* data = new fann_type[CHARACTER_SIZE];
	fann_type* ptr = data;
	for (int row = 0; row < MAX_CHARACTER_HEIGHT; ++row)
	{
		for (int column = 0; column < MAX_CHARACTER_WIDTH; ++column)
		{
			*ptr++ = static_cast<float>(pc.character.at<Vec3b>(row, column)[0]) == 0 ? 0: 1;
		}
	}
    return data;
}

fann_type* ConversionTools::letter_to_data(char letter) {
	letter = toupper(letter);

	fann_type* data = new fann_type[sizeof(possible_characters)];
	for (int i = 0; i < sizeof(possible_characters); i++) {
		if (letter == possible_characters[i]) {
			data[i] = 1;
		} else {
			data[i] = 0;
		}
	}
	return data;
}

void ConversionTools::print_data(fann_type* data, int data_size) {
	for (int i = 0; i < data_size; i++) {
		cout << (data[i] == 0 ? 0 : 1) << " ";
	}
	cout << endl;
}

void ConversionTools::print_letter_ascii(PlateCharacter &pc) {
	for (int row = 0; row < MAX_CHARACTER_HEIGHT; ++row)
	{
		for (int column = 0; column < MAX_CHARACTER_WIDTH; ++column)
		{
			std::cout << static_cast<float>(pc.character.at<Vec3b>(row, column)[0]) / 255.f << " ";
		}
		std:cout << endl;
	}
}

char ConversionTools::codeToCharacter(char code) {
	return possible_characters[code];
}

int ConversionTools::findBestMatchIndex(fann_type *output) {
	fann_type highest_propability = 0;
	int best_match_index = -1;

	for(int i = 0; i < POSSIBLE_CHARACTERS_COUNT; ++i) {
		if (output[i] > highest_propability) {
			best_match_index = i;
			highest_propability = output[i];
		}
	}
	return best_match_index;
}