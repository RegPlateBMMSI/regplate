#include <stdio.h>
#include <ctype.h>

#include "fann.h"

#include "../common/ImagePreprocessing.h"
#include "../common/PlateCharacter.h"
#include "../common/ConversionTools.h"

#define MAX_LAYERS 7

int main(int argc, char *argv[])
{
	int num_layers;
	int num_neurons_hidden;
	float desired_error;
	unsigned int layers[MAX_LAYERS];
	if(argc < 4) {
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
	struct fann_train_data *train_data, *test_data;

	printf("Creating network.\n");

	train_data = fann_read_train_from_file("regplate.train");

	ann = fann_create_standard_array(num_layers, layers);

	printf("Training network.\n");

	fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL);
	fann_set_learning_momentum(ann, 0.4f);

	fann_train_on_data(ann, train_data, 3000, 10, desired_error);

	printf("Testing network.\n");

	test_data = fann_read_train_from_file("regplate.test");

	fann_reset_MSE(ann);
	for(int i = 0; i < fann_length_train_data(test_data); i++)
	{
		fann_test(ann, test_data->input[i], test_data->output[i]);
	}
	printf("MSE error on test data: %f\n", fann_get_MSE(ann));

	printf("Saving network.\n");

	fann_save(ann, "regplate.net");

	printf("Cleaning up.\n");
	fann_destroy_train(train_data);
	fann_destroy_train(test_data);
	fann_destroy(ann);

	return 0;
}
