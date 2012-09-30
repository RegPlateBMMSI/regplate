#include "ConversionTools.h"


/**
 * Mo�liwe znaki, kt�re mog� pojawi� si� na tablicy rejestracyjnej
 */	
char possible_characters[POSSIBLE_CHARACTERS_COUNT] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};


/**
 * wyci�ga numer tablicy z nazwy pliku w formacie "photos/{numer}.jpg"
 */
std::string ConversionTools::getLicensePlateNumberFromPhotoFilename(char* filename) {
	int size = strlen(filename) - strlen("photos/") - strlen(".jpg");
	return std::string(filename + strlen("photos/"), size);
}

/**
 * Por�wnuje, czy liczba znak�w znalezionych na zdj�ciu zgadza si� z liczb� znak�w w numerze tablicy
 *
 * @param images Lista znalezionych znak�w
 * @param licenseStringNumber numer tablicy
 *
 * @return Prawda, je�li liczby s� r�wne
 */
bool ConversionTools::checkFoundCharactersCount(std::vector<Mat> images, string licensePlateNumber) {
	if (images.size() != licensePlateNumber.length()) {
		cerr << "Nieprawidlowa liczba znalezionych znakow (" << images.size() << ") w pliku: " << licensePlateNumber << endl;
		return false;
	} else {
		return true;
	}
}

/**
 * Dokonuje serializacji z obrazka ze znakiem na tablic� zer i jedynek
 *
 * @param pc Znak na tablicy (obrazek)
 *
 * @return Tablica zer i jedynek reprezentuj�cych znak
 */
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

/**
 * Dokonuje serializacji znaku na tablicy na tablic� z 1 na pozycji odpowiadaj�cej znakowi
 * w possible_characters, oraz reszt� element�w jako 0
 *
 * @param letter Znak
 *
 * @return Tablica zer i jedynek odpowiadaj�ca znakowi
 */
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

/**
 * Wy�wietla zera lub jedynki z tablicy, oddzielone spacjami
 */
void ConversionTools::print_data(fann_type* data, int data_size) {
	for (int i = 0; i < data_size; i++) {
		cout << (data[i] == 0 ? 0 : 1) << " ";
	}
	cout << endl;
}

/**
 * Zwraca liter� na pozycji `code` w tablicy possible_characters
 */
char ConversionTools::codeToCharacter(char code) {
	return possible_characters[code];
}

/**
 * W tablicy wynik�w zwr�conej przez FANN znajduje wynik o najwy�szym prawdopodobie�stwie
 * 
 * @param output Tablica prawdopodobie�stw
 * 
 * @return Indeks znaku o najwy�szym prawdopodobie�swie, lub -1 je�li wszystkie prawdopodobie�stwa s� r�wne 0
 */
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