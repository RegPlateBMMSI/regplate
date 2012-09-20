#pragma once

#include <string>
#include "ImagePreprocessing.h"
#include "PlateCharacter.h"

#define POSSIBLE_CHARACTERS_COUNT 36

class ConversionTools {
public:
	static std::string getLicensePlateNumberFromPhotoFilename(char* filename);
	static char* plate_character_to_data(PlateCharacter &pc);
	static char* letter_to_data(char letter);
	static void print_data(char* data, int data_size);
};
