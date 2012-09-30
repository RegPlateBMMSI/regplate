#pragma once

#include <string>
#include "ImagePreprocessing.h"
#include "PlateCharacter.h"
#include "fann.h"

#define POSSIBLE_CHARACTERS_COUNT 36

class ConversionTools {
public:
	static std::string getLicensePlateNumberFromPhotoFilename(char* filename);
	static fann_type* plate_character_to_data(PlateCharacter &pc);
	static fann_type* letter_to_data(char letter);
	static void print_data(fann_type* data, int data_size);
	static bool checkFoundCharactersCount(std::vector<Mat> images, string licensePlateNumber);
	static char codeToCharacter(char code);
    static int findBestMatchIndex(fann_type *output);
};
