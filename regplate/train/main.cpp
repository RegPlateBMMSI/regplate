#include <stdio.h>

#include "fann.h"

int main(int argc, char *argv[])
{
	int num_layers;
	int num_neurons_hidden;
	float desired_error;
	if(argc == 4) {
		num_layers = atoi(argv[1]);
		num_neurons_hidden = atoi(argv[2]);
		desired_error = atof(argv[3]);
	}
	else {
		printf("[ERROR] No network parameters specified. Closing...\n");
		return -1;
	}
	
	struct fann *ann;
	struct fann_train_data *train_data, *test_data;

	unsigned int i = 0;

	printf("Creating network.\n");

	train_data = fann_read_train_from_file("regplate.train");

	ann = fann_create_standard(num_layers,
					  train_data->num_input, num_neurons_hidden, train_data->num_output);

	printf("Training network.\n");

	fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL);
	fann_set_learning_momentum(ann, 0.4f);

	fann_train_on_data(ann, train_data, 3000, 10, desired_error);

	printf("Testing network.\n");

	test_data = fann_read_train_from_file("regplate.test");

	fann_reset_MSE(ann);
	for(i = 0; i < fann_length_train_data(test_data); i++)
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
