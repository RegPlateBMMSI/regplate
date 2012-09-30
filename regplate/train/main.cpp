#include <stdio.h>
#include <ctype.h>

#include "fann.h"

#include "../common/ImagePreprocessing.h"
#include "../common/PlateCharacter.h"
#include "../common/ConversionTools.h"

#define MAX_LAYERS 7 // maksymalna liczba warstw sieci

int main(int argc, char *argv[])
{
	// odczytuje z argumentów wywo³ania programu kolejno oczekiwany b³¹d, liczbê wartw i liczby neuronów na warstwach ukrytych
	int num_layers;
	int num_neurons_hidden;
	float desired_error;
	unsigned int layers[MAX_LAYERS];
	if(argc < 3) {
		printf("[ERROR] Missing network parameters. Closing...\n");
		return -1;
	}

	desired_error = atof(argv[1]);
	num_layers = atoi(argv[2]);

	if (num_layers > MAX_LAYERS || argc != num_layers + 1) {
		printf("[ERROR] Too many layers or some of layers were not specified. Closing...\n");
		return -1;
	}

	layers[0] = CHARACTER_SIZE;
	for(int i = 0; i < num_layers - 2; i++) {
		layers[i+1] = atoi(argv[3+i]);
	}
	layers[num_layers-1] = POSSIBLE_CHARACTERS_COUNT;

	struct fann *ann;
	struct fann_train_data *train_data;

	printf("Creating network.\n");

	train_data = fann_read_train_from_file("regplate.train");

	ann = fann_create_standard_array(num_layers, layers);

	printf("Training network.\n");

	fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL); // ustawia algorytm uczenia sieci
	fann_set_learning_momentum(ann, 0.4f);

	fann_train_on_data(ann, train_data, 3000, 10, desired_error); // dokonuje uczenia sieci

	printf("Saving network.\n");
	fann_save(ann, "regplate.net"); // zapisuje sieæ do pliku

	printf("Cleaning up.\n");
	fann_destroy_train(train_data);
	fann_destroy(ann);

	return 0;
}
